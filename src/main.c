#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include "render.h"
#include "client.h"
#include "server.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [server|client]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "server") == 0) {
        return run_server();  // Exit after running the server
    } else if (strcmp(argv[1], "client") == 0) {
        // Proceed with SDL initialization and client logic
    } else {
        fprintf(stderr, "Invalid argument. Use 'server' or 'client'.\n");
        return 1;
    }

    // SDL initialization
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    const int frameDelay = 1000 / 60;
    Uint32 frameStart = SDL_GetTicks();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialise SDL: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("fonts/Roboto-Thin.ttf", 24);
    if (!font)
    {
        printf("Font didn't load :-()");
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Chat Client",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                         1280, 800, SDL_WINDOW_SHOWN);
    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Color color = {255, 255, 255, 255};

    char *text = "whats up guys";

    bool running = true;
    SDL_Event event;

    int sockfd = conn_setup();

    char username[100];
    char formatted_username[104];
    printf("Enter username: ");
    if (fgets(username, sizeof(username), stdin)) {
        username[strcspn(username, "\n")] = '\0';
        snprintf(formatted_username, sizeof(formatted_username), "%s%s", username, ": ");
    } else {
        printf("Error in username input");
    }


    while (running)
    {
        frameStart = SDL_GetTicks(); 

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_Rect textbox = {50, 50, 500, 300};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Grey background
        SDL_RenderFillRect(renderer, &textbox);

        SDL_Texture* textTexture = renderText(renderer, font, text, color);
         if (textTexture) {
            SDL_Rect textRect = {60, 60, 0, 0}; // Position text inside the textbox
            SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        client(renderer, font, sockfd, &running, formatted_username);
        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
        
    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}