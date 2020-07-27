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

enum UserAction {
    MoveLeft = 1,
    MoveRight,
    Jump,
    Attack,
    Select
};

constexpr auto MAX_KEYS_PER_ACTION = 2;
#define WHITE SDL_Color{255, 255, 255, SDL_ALPHA_OPAQUE}
#define BLACK SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE}
#define BLUE SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}
#define RED SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE}
#define GREEN SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}

std::string up_or_down(bool);
std::string bool_string(bool);
std::string mouse_button_to_name(Uint8);
const std::string useraction_to_name(Sint32 a);
bool is_colliding(SDL_Point&, SDL_Rect&);
void render_text(SDL_Renderer*, TTF_Font *, std::string, SDL_Color, int, int, int);

int input_test() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_WINDOW_RESIZABLE);
    TestState test_state;
    TTF_Init();

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
    SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
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
    ControllerList list;
    Controller c = {
        false,
        false,
        {0, 0},
        {0, 0},
        0,
        0,
        controller_map
    };
    list.emplace_back(c);

    Input input(key_map, mouse_map, list);
    Engine engine(input);

    while (!engine.input.handle_input()) {
        if (engine.input.is_pressed(Jump, true, 0)) {
            test_state.jump_toggled = !test_state.jump_toggled;
        }

        if (engine.input.is_pressed(Attack, true, 0)) {
            test_state.attack_toggled = !test_state.attack_toggled;
        }

        if (engine.input.is_pressed(MoveLeft, true, 0)) {
            test_state.left_toggled = !test_state.left_toggled;
        }

        if (engine.input.is_pressed(MoveRight, true, 0)) {
            test_state.right_toggled = !test_state.right_toggled;
        }

        auto mouseCoordinates = engine.input.mouse_position;
        auto mouseDelta = engine.input.mouse_delta;
        auto mouseWheel = engine.input.mouse_wheel;


        // Controller info
        render_text(renderer, font, "Controllers connected: " + std::to_string(SDL_NumJoysticks()), WHITE, 25, 625, 5);
        int j = 0;
        for (auto &controller : engine.input.controllers) {
            const Uint8 x_column = j % 2;
            const Uint8 y_row = j / 2;
            render_text(renderer, font, "Controller " + std::to_string(j), WHITE, 12, 625 + x_column * 330, 35 + 100 * y_row);
            render_text(renderer, font, "Left horizontal: " + std::to_string(controller.left.horizontal_axis), WHITE, 12, 625 + x_column * 330, 50 + 100 * y_row);
            render_text(renderer, font, "Left vertical: " + std::to_string(controller.left.vertical_axis), WHITE, 12, 625 + x_column * 330, 70 + 100 * y_row);
            render_text(renderer, font, "Right horizontal: " + std::to_string(controller.right.horizontal_axis), WHITE, 12, 625 + x_column * 330, 90 + 100 * y_row);
            render_text(renderer, font, "Right vertical: " + std::to_string(controller.right.vertical_axis), WHITE, 12, 625 + x_column * 330, 110 + 100 * y_row);
            render_text(renderer, font, "Left trigger: " + std::to_string(controller.left_trigger), WHITE, 12, 625 + x_column * 330, 130 + 100 * y_row);
            render_text(renderer, font, "Right trigger: " + std::to_string(controller.right_trigger), WHITE, 12, 625 + x_column * 330, 150 + 100 * y_row);
            j++;
        }

        // For each action, show bound key;
        // On click, set rebind for a key.
        render_text(renderer, font, std::string("Jump toggled ") + bool_string(test_state.jump_toggled), WHITE, 25, 10, 5);
        render_text(renderer, font, std::string("Attack toggled ") + bool_string(test_state.attack_toggled), WHITE, 25, 10, 40);
        render_text(renderer, font, std::string("Left toggled ") + bool_string(test_state.left_toggled), WHITE, 25, 10, 75);
        render_text(renderer, font, std::string("Right toggled ") + bool_string(test_state.right_toggled), WHITE, 25, 10, 110);

        render_text(renderer, font, "Jump " + up_or_down(engine.input.is_held(UserAction::Jump, true, 0)), WHITE, 25, 10, 145);
        render_text(renderer, font, "Attack " + up_or_down(engine.input.is_held(UserAction::Attack, true, 0)), WHITE, 25, 10, 180);
        render_text(renderer, font, "Left " + up_or_down(engine.input.is_held(UserAction::MoveLeft, true, 0)), WHITE, 25, 10, 215);
        render_text(renderer, font, "Right " + up_or_down(engine.input.is_held(UserAction::MoveRight, true, 0)), WHITE, 25, 10, 250);
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
                if (SDL_PointInRect(&mouseCoordinates, &rect) && engine.input.is_mouse_button_pressed(SDL_BUTTON_LEFT)) {
                    std::cout << "Preparing to rebind " << action_name << " with intention to erase " << SDL_GetKeyName(key_pair.first) << std::endl;
                    engine.input.start_rebind_keyboard_action((UserAction) act, key_pair.first);
                }
            }

            for (auto &mouse_pair : engine.input.mouse_map) {
                if (mouse_pair.second != act || mapped_count == 0) continue;
                mapped_count--;
                SDL_Rect rect{ 250 - 4 + j * 250, y - 4, 240, 32 };
                SDL_RenderDrawRect(renderer, &rect);
                render_text(renderer, font, mouse_button_to_name(mouse_pair.first), WHITE, 25, 250 + j * 250, y);
                j++;
                if (SDL_PointInRect(&mouseCoordinates, &rect) && engine.input.is_mouse_button_pressed(SDL_BUTTON_LEFT)) {
                    engine.input.start_rebind_mouse_action((UserAction)act, (Uint8) mouse_pair.first);
                }
            }

            if (mapped_count) {
                for (; mapped_count > 0; mapped_count--) {
                    render_text(renderer, font, "Unmapped", WHITE, 25, 250 + j * 250, y);
                    SDL_Rect rect{ 250 - 4 + j * 250, y - 4, 240, 32 };
                    SDL_RenderDrawRect(renderer, &rect);
                    j++;

                    if (SDL_PointInRect(&mouseCoordinates, &rect) && engine.input.is_mouse_button_pressed(SDL_BUTTON_LEFT)) {
                        std::cout << "Preparing to rebind " << action_name << " with intention to write to unmapped." << std::endl;
                        engine.input.start_rebind_keyboard_action((UserAction)act, SDLK_UNKNOWN);
                    }
                }
            }

            int button = -1;

            for (auto& controller_pair : engine.input.controllers[0].controller_map) {
                if (controller_pair.second == act) {
                    button = (int) controller_pair.first;
                }
            }

            
            const auto button_text = button == -1 ? "Unmapped" : SDL_GameControllerGetStringForButton((SDL_GameControllerButton)button);
            render_text(renderer, font, button_text, WHITE, 25, 250 + j * 250, y);
            SDL_Rect rect{ 250 - 4 + j * 250, y - 4, 240, 32 };
            SDL_RenderDrawRect(renderer, &rect);
            j++;

            if (SDL_PointInRect(&mouseCoordinates, &rect) && engine.input.is_mouse_button_pressed(SDL_BUTTON_LEFT)) {
                engine.input.start_rebind_action_controller((UserAction)act, 0, button);
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
   SDL_Rect position = { x, y, (int) floor(font_size * text.length()), font_size };
   SDL_RenderCopy(renderer, texture, NULL, &position);
   SDL_DestroyTexture(texture);
   SDL_FreeSurface(textSurface);
}

std::string mouse_button_to_name(Uint8 button) {
    switch (button) {
        case SDL_BUTTON_LEFT:
            return "Mouse Left";
        case SDL_BUTTON_RIGHT:
            return "Mouse Right";
        case SDL_BUTTON_MIDDLE:
            return "Mouse Middle";
        case SDL_BUTTON_X1:
            return "Mouse X1";
        case SDL_BUTTON_X2:
            return "Mouse X2";
        default:
            return "Unhandled mouse button";
    }
}

std::string up_or_down(bool value) {
    return value ? "down" : "up";
}

std::string bool_string(bool value) {
    return value ? "true" : "false";
}

const std::string useraction_to_name(Sint32 a) {
    switch (a) {
        case UserAction::MoveLeft:
            return "MoveLeft";
        case UserAction::MoveRight:
            return "MoveRight";
        case UserAction::Jump:
            return "Jump";
        case UserAction::Attack:
            return "Attack";
        default:
            return "Not mapped";
    }
}
