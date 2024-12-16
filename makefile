CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

# SDL2 config
SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_ttf

SRC = main.c render.c
TARGET = chat_client

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SDL2_CFLAGS) $(SRC) -o $(TARGET) $(SDL2_LDFLAGS)

clean:
	rm -f $(TARGET)