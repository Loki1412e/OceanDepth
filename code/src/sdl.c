#include "../include/sdl.h"

int initSDL(SDL_Window** window, SDL_Renderer** renderer);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

TTF_Font* loadFont(const char* fontPath, int fontSize);

void renderBlackScreen(SDL_Renderer* renderer);
void renderMenu(SDL_Renderer* renderer, TTF_Font* font, char** options, size_t nbOptions, size_t selected);


#include "../include/sdl.h"

int initSDL(SDL_Window** window, SDL_Renderer** renderer) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    *window = SDL_CreateWindow(
        "SDL2 Noir + Console",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 480, 0
    );
    
    if (!*window) {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Initialisation des autres sous-systèmes SDL (image, mixer, ttf)
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur IMG_Init: %s\n", IMG_GetError());
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erreur Mix_OpenAudio: %s\n", Mix_GetError());
        IMG_Quit();
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        Mix_CloseAudio();
        IMG_Quit();
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer) {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    TTF_Quit();
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
}


TTF_Font* loadFont(const char* fontPath, int fontSize) {
    TTF_Font* font = TTF_OpenFont(fontPath, fontSize);
    if (!font) {
        fprintf(stderr, "Erreur chargement police (%s): %s\n", fontPath, TTF_GetError());
    }
    return font;
}


void renderBlackScreen(SDL_Renderer* renderer) {
    if (!renderer) return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // noir opaque
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void renderMenu(SDL_Renderer* renderer, TTF_Font* font, char** options, size_t nbOptions, size_t selected) {
    SDL_Color colorNormal = {255, 255, 255, 255};  // blanc
    SDL_Color colorSelected = {255, 255, 0, 255};  // jaune

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // fond noir
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < nbOptions; i++) {
        SDL_Color color = (i == selected) ? colorSelected : colorNormal;
        SDL_Surface* surface = TTF_RenderText_Solid(font, options[i], color);
        if (!surface) {
            SDL_Log("Erreur TTF_RenderText_Solid : %s", TTF_GetError());
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_Log("Erreur SDL_CreateTextureFromSurface : %s", SDL_GetError());
            SDL_FreeSurface(surface);
            continue;
        }
        SDL_Rect dstRect = {50, 50 + (int)i * 40, surface->w, surface->h};
        SDL_FreeSurface(surface);

        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}
