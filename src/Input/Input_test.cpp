#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "Input.hpp"
#include "../GameState.h"

using namespace std;

int input_test() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    auto *font = TTF_OpenFont("/assets/Font/PressStart2P.ttf", 32);
    if (font == NULL) {
        cout << "Failed to load font" << endl;
    }
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    KeyMap map;
    auto &mapRef = map;
    Input input(mapRef);
    GameState game_state(input);

    string moveLeft = "";
    string moveRight = "";
    string jump = "";
    string attack = "";

    while (true) {
        game_state.input.handle_input();

        auto mouseCoordinates = game_state.input.mouse_position;
        // For each action, show bound key;
        // On click, set rebind for a key.


        string action_jump = "Jump";
        
        
        

        render_text_from_text(renderer, font, "");

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void render_text_from_text(SDL_Renderer *renderer, TTF_Font *font, string text, SDL_Color c, SDL_Rect position) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Color", SDL_Color({255, 255, 255, SDL_ALPHA_OPAQUE}));
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    free(textSurface);
    SDL_RenderCopy(renderer, texture, NULL, &position);
}