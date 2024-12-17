#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include "addr.h"


#define PORT "9034"
#define MAXDATASIZE 100
#define MAX_MESSAGES 14
#define MAX_MESSAGE_LENGTH 256

char chat_messages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
int message_count = 0;
int message_start = 0;

void add_message(char *new_message) { // Circular buffer to hold messages
    if (message_count < MAX_MESSAGES) {

        strncpy(chat_messages[message_count], new_message, MAX_MESSAGE_LENGTH - 1);
        chat_messages[message_count][MAX_MESSAGE_LENGTH - 1] = '\0'; 
        message_count++;
    } else {

        strncpy(chat_messages[message_start], new_message, MAX_MESSAGE_LENGTH - 1);
        chat_messages[message_start][MAX_MESSAGE_LENGTH - 1] = '\0';
        message_start = (message_start + 1) % MAX_MESSAGES;
    }
}

// Establish connection to the server
int connect_to_server()
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    // Loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return -1;
    }

    freeaddrinfo(servinfo); // All done with this structure
    return sockfd;
}

void client_conn(int sockfd, char **output_text, bool *running) {
    char buf[MAXDATASIZE];
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    int max_fd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    if (select(max_fd + 1, &read_fds, NULL, NULL, &timeout) == -1) {
        perror("select");
        *running = false;
        return;
    }

    // Check if the server sent a message
    if (FD_ISSET(sockfd, &read_fds)) {
        int numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);

        if (numbytes <= 0) {
            if (numbytes == 0) {
                printf("Server closed the connection.\n");
            } else {
                perror("recv");
            }
            *running = false;
            return;
        }

        buf[numbytes] = '\0';

        // update output_text
        if (*output_text) {
            free(*output_text); // Free existing memory
        }
        *output_text = strdup(buf); // assign received text to output_text
        if (!*output_text) {
            perror("strdup");
            *running = false;
            return;
        }
    }

    // Check for user input
    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
        char input[MAXDATASIZE];
        ssize_t bytes_read = read(STDIN_FILENO, input, sizeof(input) - 1);

        if (bytes_read < 0) {
        perror("read");
        *running = false;
        return;
    }

    if (bytes_read == 0) {
        printf("Goodbye!\n");
        *running = false;
        return;
    }

    if (bytes_read == 1 && input[0] == '\n') { // Check for empty STDIN input
        printf("Please type one or more characters");
        return;
    }

    // Null-terminate the input
    input[bytes_read] = '\0';

    // Remove the newline character, if present
    char *newline = strchr(input, '\n');
    if (newline) {
        *newline = '\0';
    }

    // Free current output_text
    if (*output_text) {
        free(*output_text);
    }

    // Update output_text with new chat value
    *output_text = strdup(input);
    if (!*output_text) {
        perror("strdup");
        *running = false;
        return;
    }
        // send new output_text to server
        if (send(sockfd, input, strlen(input), 0) == -1) {
            perror("send");
            *running = false;
        }

        add_message(*output_text);
    }
}


// Render chat text to window
void render_chat(SDL_Renderer *renderer, TTF_Font *font, char name[]) {
    
    SDL_Color color = {255, 255, 255, 255};
    SDL_Rect textbox = {50, 50, 500, 300};

    // Draw grey textbox background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &textbox);

    int y_offset = 0;
    int line_height = 20;

    // Render all chat messages
    for (int i = 0; i < message_count; i++) {
        int msg_index = (message_start + i) % MAX_MESSAGES;
        char named_message[356];
        snprintf(named_message, sizeof(named_message), "%s%s", name, chat_messages[msg_index]);
        SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, named_message, color, textbox.w - 10);
        if (surface) {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {textbox.x + 5, textbox.y + y_offset + 5, surface->w, surface->h};
            y_offset += line_height;
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        } else {
            printf("Text rendering failed: %s\n", TTF_GetError());
        }
    }
}

int conn_setup() {
    int sockfd = connect_to_server();
    if (sockfd == -1) {
        fprintf(stderr, "Failed to initialize client connection.\n");
        return -1;
    }
    return sockfd;
}

void client(SDL_Renderer *renderer, TTF_Font *font, int sockfd, bool *running, char name[])
{
    static char *text = NULL; // Holds last received/sent message

    // Handle client-side connection (non-blocking I/O)
    client_conn(sockfd, &text, running);

    // Render chat messages
    render_chat(renderer, font, name);

    // Free text buffer on cleanup
    if (!*running && text) {
        free(text);
        text = NULL;
    }
}
