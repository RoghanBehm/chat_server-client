#include "render.h"

SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) {
        printf("TTF_RenderText_Blended Error: %s\n", TTF_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    }

    return texture;
}
