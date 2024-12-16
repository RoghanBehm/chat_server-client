# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c17 -D_POSIX_C_SOURCE=200809L -Iinclude -I/usr/include/SDL2 -D_REENTRANT -g
LDFLAGS = -lSDL2 -lSDL2_ttf


# SDL2 configuration
SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_ttf

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Target and source files
TARGET = $(BIN_DIR)/chat_client
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Default rule
all: $(TARGET)

# Rule to link the final binary
$(TARGET): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(SDL2_CFLAGS) $^ -o $@ $(SDL2_LDFLAGS)


# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(SDL2_CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
