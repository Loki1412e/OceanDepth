#ifndef _SDL_H_
#define _SDL_H_

    #include "global.h"

    int initSDL(SDL_Window** window, SDL_Renderer** renderer);
    void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

    TTF_Font* loadFont(const char* fontPath, int fontSize);

    void renderBlackScreen(SDL_Renderer* renderer);
    void renderMenu(SDL_Renderer* renderer, TTF_Font* font, char** options, size_t nbOptions, size_t selected);

#endif
