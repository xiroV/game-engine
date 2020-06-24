#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include "Input.hpp"
#include "../Engine.h"

struct TestState {
    bool jump_toggled = false;
    bool left_toggled = false;
    bool right_toggled = false;
    bool attack_toggled = false;
};

#define MAX_KEYS_PER_ACTION 2
#define WHITE SDL_Color{255, 255, 255, SDL_ALPHA_OPAQUE}
#define BLACK SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE}
#define BLUE SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}
#define RED SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE}
#define GREEN SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}

std::string up_or_down(bool value);
std::string bool_string(bool value);
bool is_colliding(SDL_Point&, SDL_Rect&);
void render_text(SDL_Renderer*, TTF_Font *, std::string, SDL_Color, int, int, int);

int input_test() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_WINDOW_RESIZABLE);
    TestState test_state;
    TTF_Init();
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            char* mapping;
            SDL_Log("Index \'%i\' is a compatible controller, named \'%s\'", i, SDL_GameControllerNameForIndex(i));
            auto ctrl = SDL_GameControllerOpen(i);
            mapping = SDL_GameControllerMapping(ctrl);
            SDL_Log("Controller %i is mapped as \"%s\".", i, mapping);
            SDL_free(mapping);
        } else {
            SDL_Log("Index \'%i\' is not a compatible controller.", i);
        }
    }

    #ifdef __WIN32__
        auto *font = TTF_OpenFont(".\\assets\\Font\\PressStart2P.ttf", 32);
    #else
        auto* font = TTF_OpenFont("./assets/Font/PressStart2P.ttf", 32);
    #endif // __WIN32__


    if (font == NULL) {
        std::cout << "Failed to load font" << std::endl;
        std::cout << TTF_GetError() << std::endl;
        return 1;
    }
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_RenderSetLogicalSize(renderer, 1280, 720);    
SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    KeyMap key_map;
    key_map[SDLK_LEFT] = MoveLeft;
    key_map[SDLK_RETURN] = Attack;
    key_map[SDLK_RIGHT] = MoveRight;
    key_map[SDLK_SPACE] = Jump;
    key_map[SDLK_LCTRL] = Attack;

    MouseMap mouse_map;
    mouse_map[SDL_BUTTON_LEFT] = Select;

    ControllerMap controller_map;
    controller_map[SDL_CONTROLLER_BUTTON_B] = Attack;
    controller_map[SDL_CONTROLLER_BUTTON_A] = Jump;

    Input input(key_map, mouse_map, controller_map);
    Engine engine(input);

    while (!engine.input.handle_input()) {
        if (engine.input.is_pressed_once(Jump)) {
            test_state.jump_toggled = !test_state.jump_toggled;
        }

        if (engine.input.is_pressed_once(Attack)) {
            test_state.attack_toggled = !test_state.attack_toggled;
        }

        if (engine.input.is_pressed_once(MoveLeft)) {
            test_state.left_toggled = !test_state.left_toggled;
        }

        if (engine.input.is_pressed_once(MoveRight)) {
            test_state.right_toggled = !test_state.right_toggled;
        }

        auto mouseCoordinates = engine.input.mouse_position;
        auto mouseDelta = engine.input.mouse_delta;
        auto mouseWheel = engine.input.mouse_wheel;
        // For each action, show bound key;
        // On click, set rebind for a key.
        render_text(renderer, font, std::string("Jump toggled ") + bool_string(test_state.jump_toggled), WHITE, 25, 10, 5);
        render_text(renderer, font, std::string("Attack toggled ") + bool_string(test_state.attack_toggled), WHITE, 25, 10, 40);
        render_text(renderer, font, std::string("Left toggled ") + bool_string(test_state.left_toggled), WHITE, 25, 10, 75);
        render_text(renderer, font, std::string("Right toggled ") + bool_string(test_state.right_toggled), WHITE, 25, 10, 110);

        render_text(renderer, font, "Jump " + up_or_down(engine.input.is_down(UserAction::Jump)), WHITE, 25, 10, 145);
        render_text(renderer, font, "Attack " + up_or_down(engine.input.is_down(UserAction::Attack)), WHITE, 25, 10, 180);
        render_text(renderer, font, "Left " + up_or_down(engine.input.is_down(UserAction::MoveLeft)), WHITE, 25, 10, 215);
        render_text(renderer, font, "Right " + up_or_down(engine.input.is_down(UserAction::MoveRight)), WHITE, 25, 10, 250);
        render_text(renderer, font, "Mouse pos x: " + std::to_string(mouseCoordinates.x), WHITE, 25, 10, 285);
        render_text(renderer, font, "Mouse pos y: " + std::to_string(mouseCoordinates.y), WHITE, 25, 10, 320);
        render_text(renderer, font, "Mouse delta x: " + std::to_string(mouseDelta.x), WHITE, 25, 10, 355);
        render_text(renderer, font, "Mouse delta y: " + std::to_string(mouseDelta.y), WHITE, 25, 10, 390);
        render_text(renderer, font, "Mouse scroll x: " + std::to_string(mouseWheel.x), WHITE, 25, 10, 425);
        render_text(renderer, font, "Mouse scroll y: " + std::to_string(mouseWheel.y), WHITE, 25, 10, 465);
        render_text(renderer, font, "Action", GREEN, 25, 10, 500);
        render_text(renderer, font, "Key", GREEN, 25, 250, 500);
        render_text(renderer, font, "Key 2", GREEN, 25, 500, 500);
        render_text(renderer, font, "Controller", GREEN, 25, 750, 500);

        int i = 0;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int act = 1; act < (int) UserAction::Select; act++) {
            const auto action_name = useraction_to_name((UserAction) act);
            int mapped_count = MAX_KEYS_PER_ACTION;
            auto y = 535 + 35 * i;
            render_text(renderer, font, action_name, WHITE, 25, 10, y);
            int j = 0;
            for (auto &key_pair : engine.input.key_map) {
                if (key_pair.second != act || mapped_count == 0) continue;
                mapped_count--;
                SDL_Rect rect{ 250 - 4 + j * 250, y - 4, 240, 32 };
                SDL_RenderDrawRect(renderer, &rect);
                render_text(renderer, font, SDL_GetKeyName(key_pair.first), WHITE, 25, 250 + j * 250, y);
                j++;
                if (SDL_PointInRect(&mouseCoordinates, &rect) && engine.input.mouse_clicked.left_mouse_button) {
                    std::cout << "Preparing to rebind " << action_name << " with intention to erase " << SDL_GetKeyName(key_pair.first) << std::endl;
                    engine.input.set_action_to_rebind((UserAction)act, RebindingDevice::KeyboardAndMouse, key_pair.first);
                }
            }

            for (auto &mouse_pair : engine.input.mouse_map) {
                if (mouse_pair.second != act || mapped_count == 0) continue;
                mapped_count--;
                SDL_Rect rect{ 250 - 4 + j * 250, y - 4, 240, 32 };
                SDL_RenderDrawRect(renderer, &rect);
                render_text(renderer, font, SDL_GetKeyName(mouse_pair.first), WHITE, 25, 250 + j * 250, y);
                j++;
                if (SDL_PointInRect(&mouseCoordinates, &rect) && engine.input.mouse_clicked.left_mouse_button) {
                    engine.input.set_action_to_rebind((UserAction)act, RebindingDevice::KeyboardAndMouse, (Uint8) mouse_pair.first);
                }
            }



            if (mapped_count) {
                for (; mapped_count > 0; mapped_count--) {
                    render_text(renderer, font, "Unmapped", WHITE, 25, 250 + j * 250, y);
                    SDL_Rect rect{ 250 - 4 + j * 250, y - 4, 240, 32 };
                    SDL_RenderDrawRect(renderer, &rect);
                    j++;

                    if (SDL_PointInRect(&mouseCoordinates, &rect) && engine.input.mouse_clicked.left_mouse_button) {
                        engine.input.set_action_to_rebind((UserAction)act, RebindingDevice::KeyboardAndMouse, SDLK_UNKNOWN);
                    }
                }
            }
            i++;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

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
   SDL_Rect position = { x, y, (int) (font_size * text.length()), font_size };
   SDL_RenderCopy(renderer, texture, NULL, &position);
   SDL_DestroyTexture(texture);
   SDL_FreeSurface(textSurface);
}

std::string up_or_down(bool value) {
    return value ? "down" : "up";
}

std::string bool_string(bool value) {
    return value ? "true" : "false";
}
