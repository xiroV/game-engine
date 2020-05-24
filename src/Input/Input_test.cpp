#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "Input.hpp"
#include "../GameState.h"

struct TestState {
    bool jump_toggled = false;
    bool left_toggled = false;
    bool right_toggled = false;
    bool attack_toggled = false;
};

std::string boolean_to_string(bool value);
void render_text(SDL_Renderer *renderer, TTF_Font *font, std::string text, SDL_Color color, int font_size, int x, int y);

int input_test() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    TestState test_state;
    auto *font = TTF_OpenFont("/assets/Font/PressStart2P.ttf", 32);
    if (font == NULL) {
        std::cout << "Failed to load font" << std::endl;
    }
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    KeyMap key_map;
    auto &mapRef = key_map;
    KeyPressMap keys_held_down;
    auto held_down_ref = keys_held_down;
    KeyPressMap keys_pressed_once;

    Input input(mapRef, keys_held_down, keys_pressed_once);
    GameState game_state(input);

    while (true) {
        game_state.input.handle_input();
                
        if (game_state.input.is_pressed_once(Jump)) {
            test_state.jump_toggled = !test_state.jump_toggled;
        }

        if (game_state.input.is_pressed_once(Attack)) {
            test_state.attack_toggled = !test_state.attack_toggled;
        }

        if (game_state.input.is_pressed_once(MoveLeft)) {
            test_state.left_toggled = !test_state.left_toggled;
        }

        if (game_state.input.is_pressed_once(MoveRight)) {
            test_state.right_toggled = !test_state.right_toggled;
        }

        auto mouseCoordinates = game_state.input.mouse_position;
        // For each action, show bound key;
        // On click, set rebind for a key.
        render_text(renderer, font, std::string("Jump toggled ") + boolean_to_string(test_state.jump_toggled), { 255, 255, 255, SDL_ALPHA_OPAQUE }, 25, 5, 5);
        render_text(renderer, font, std::string("Attack toggled ") + boolean_to_string(test_state.attack_toggled), { 255, 255, 255, SDL_ALPHA_OPAQUE }, 25, 5, 30);
        render_text(renderer, font, std::string("Left toggled ") + boolean_to_string(test_state.left_toggled), { 255, 255, 255, SDL_ALPHA_OPAQUE }, 25, 5, 55);
        render_text(renderer, font, std::string("Right toggled ") + boolean_to_string(test_state.right_toggled), { 255, 255, 255, SDL_ALPHA_OPAQUE }, 25, 5, 80);


        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, std::string text, SDL_Color color, int font_size, int x, int y) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    free(textSurface);
    SDL_Rect position = { x, y, (int) (font_size * text.length()), font_size };
    SDL_RenderCopy(renderer, texture, NULL, &position);
}

std::string boolean_to_string(bool value) {
    return value ? "true" : "false";
}