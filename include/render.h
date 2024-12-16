#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color);

#endif
