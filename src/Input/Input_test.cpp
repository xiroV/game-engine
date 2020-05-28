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

#define WHITE SDL_Color{255, 255, 255, SDL_ALPHA_OPAQUE}
#define BLACK SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE}
#define BLUE SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}

std::string up_or_down(bool value);
std::string bool_string(bool value);
bool is_colliding(SDL_Point&, SDL_Rect&);
void render_text(SDL_Renderer*, TTF_Font *, std::string, SDL_Color, int, int, int);

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
        auto mouseDelta = game_state.input.mouse_delta;
        // For each action, show bound key;
        // On click, set rebind for a key.
        render_text(renderer, font, std::string("Jump toggled ") + bool_string(test_state.jump_toggled), WHITE, 25, 10, 5);
        render_text(renderer, font, std::string("Attack toggled ") + bool_string(test_state.attack_toggled), WHITE, 25, 10, 40);
        render_text(renderer, font, std::string("Left toggled ") + bool_string(test_state.left_toggled), WHITE, 25, 10, 75);
        render_text(renderer, font, std::string("Right toggled ") + bool_string(test_state.right_toggled), WHITE, 25, 10, 110);

        render_text(renderer, font, std::string("Jump ") + up_or_down(game_state.input.is_down(Jump)), WHITE, 25, 10, 145);
        render_text(renderer, font, std::string("Attack ") + up_or_down(game_state.input.is_down(Attack)), WHITE, 25, 10, 180);
        render_text(renderer, font, std::string("Left ") + up_or_down(game_state.input.is_down(MoveLeft)), WHITE, 25, 10, 215);
        render_text(renderer, font, std::string("Right ") + up_or_down(game_state.input.is_down(MoveRight)), WHITE, 25, 10, 250);
        render_text(renderer, font, std::string("Mouse pos x: ") + std::to_string(mouseCoordinates.x), WHITE, 25, 10, 285);
        render_text(renderer, font, std::string("Mouse pos y: ") + std::to_string(mouseCoordinates.y), WHITE, 25, 10, 320);
        render_text(renderer, font, std::string("Mouse delta x: ") + std::to_string(mouseDelta.x), WHITE, 25, 10, 355);
        render_text(renderer, font, std::string("Mouse delta y: ") + std::to_string(mouseDelta.y), WHITE, 25, 10, 390);

        

         // game_state.input.key_map.
        int i = 0;
        for (auto key_action_pair : game_state.input.key_map) {
            auto action_name = useraction_to_name(key_action_pair.second);
            if (action_name != "Not mapped") {
                render_text(renderer, font, action_name + " " + SDL_GetKeyName(key_action_pair.first), WHITE, 25, 10, (425 + 35 * i)); 
                
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect rect { 600, (425 + 35 * i++), 200, 25 };
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                if (is_colliding(mouseCoordinates, rect)) {

                }
            }
        }

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

bool is_colliding(SDL_Point &point, SDL_Rect &rect) {
    return rect.x < point.x && point.x < rect.x + rect.w && rect.y < point.y && point.y < rect.y + rect.h;
}

std::string up_or_down(bool value) {
    return value ? "down" : "up";
}

std::string bool_string(bool value) {
    return value ? "true" : "false";
}