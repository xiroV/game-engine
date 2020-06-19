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
    bool select = false;
};

#define WHITE SDL_Color{255, 255, 255, SDL_ALPHA_OPAQUE}
#define BLACK SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE}
#define BLUE SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}

std::string up_or_down(bool value);
std::string bool_string(bool value);
bool is_colliding(SDL_Point&, SDL_Rect&);
void render_text(SDL_Renderer*, TTF_Font *, std::string, SDL_Color, int, int, int);

int input_test() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);
    TestState test_state;
    std::cout << SDL_GetBasePath() << std::endl;
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
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    KeyMap key_map;
    key_map[SDLK_LEFT] = MoveLeft;
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

    bool running = true;

    while (running) {
        running = !engine.input.handle_input();
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
        // For each action, show bound key;
        // On click, set rebind for a key.
        render_text(renderer, font, std::string("Jump toggled ") + bool_string(test_state.jump_toggled), WHITE, 25, 10, 5);
        render_text(renderer, font, std::string("Attack toggled ") + bool_string(test_state.attack_toggled), WHITE, 25, 10, 40);
        render_text(renderer, font, std::string("Left toggled ") + bool_string(test_state.left_toggled), WHITE, 25, 10, 75);
        render_text(renderer, font, std::string("Right toggled ") + bool_string(test_state.right_toggled), WHITE, 25, 10, 110);

        render_text(renderer, font, "Jump " + up_or_down(engine.input.is_down(Jump)), WHITE, 25, 10, 145);
        render_text(renderer, font, "Attack " + up_or_down(engine.input.is_down(Attack)), WHITE, 25, 10, 180);
        render_text(renderer, font, "Left " + up_or_down(engine.input.is_down(MoveLeft)), WHITE, 25, 10, 215);
        render_text(renderer, font, "Right " + up_or_down(engine.input.is_down(MoveRight)), WHITE, 25, 10, 250);
        render_text(renderer, font, "Mouse pos x: " + std::to_string(mouseCoordinates.x), WHITE, 25, 10, 285);
        render_text(renderer, font, "Mouse pos y: " + std::to_string(mouseCoordinates.y), WHITE, 25, 10, 320);
        render_text(renderer, font, "Mouse delta x: " + std::to_string(mouseDelta.x), WHITE, 25, 10, 355);
        render_text(renderer, font, "Mouse delta y: " + std::to_string(mouseDelta.y), WHITE, 25, 10, 390);

        int i = 0;
        for (auto key_action_pair : engine.input.key_map) {
            auto action_name = useraction_to_name(key_action_pair.second);
            if (action_name != "Not mapped") {
                render_text(renderer, font, action_name + " " + SDL_GetKeyName(key_action_pair.first), WHITE, 25, 10, (425 + 35 * i)); 
                
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect rect { 600, (425 + 35 * i), 100, 25 };
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                render_text(renderer, font, "Rebind", BLACK, 12, 615, 432 + 35 * i);
                if (SDL_PointInRect(&mouseCoordinates, &rect) && engine.input.is_pressed_once(Select)) {
                    std::cout << "Clicked. Rebinding." << std::endl;
                    engine.input.set_action_to_rebind(key_action_pair.second, RebindingDevice::Keyboard);
                }

            }
            i++;
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