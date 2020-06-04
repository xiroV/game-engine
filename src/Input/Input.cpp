#include <SDL2/SDL.h>
#include <iostream>
#include <map>
#include "Input.hpp"

bool Input::is_down(UserAction a) {
    return this->controller_held_down[a] || this->mouse_button_held[a] || this->keys_held_down[a];
}

bool Input::is_pressed_once(UserAction a) {
    return this->controller_pressed_once[a] || this->mouse_clicked_once[a] || this->keys_pressed_once[a];
}

Input::Input(
    KeyMap &key_map,
    MouseMap &mouse_map,
    ControllerMap &controller_map
): key_map{key_map}, mouse_map{mouse_map}, controller_map{controller_map} {
}

Input::~Input() {}

void Input::bind_key(SDL_Keycode key) {
    this->key_map[key] = this->rebind_action;
    std::cout << "Rebound " << useraction_to_name(this->rebind_action) << std::endl;
}

/* Shouldn't be here. */
void quit() {
    std::cout << "Quitting" << std::endl;
    exit(0);
}

void Input::set_action_to_rebind(UserAction action) {
    std::cout << "We want to rebind " << useraction_to_name(action) << std::endl;
    this->state = Rebinding;
    this->rebind_finished = false;
    this->rebind_action = action;
}

void Input::handle_input() {

    for (auto p : this->keys_pressed_once) {
        this->keys_pressed_once[p.first] = false;
    }

    for (auto p : this->controller_pressed_once) {
        this->controller_pressed_once[p.first] = false;
    }

    for (auto p : this->mouse_clicked_once) {
        this->mouse_clicked_once[p.first] = false;
    }

    SDL_Event e;
    int y = 0;
    int x = 0;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_MOUSEMOTION) {
            const auto mouse_state = SDL_GetMouseState(&x, &y);
            this->mouse_delta = {x - this->mouse_position.x, y - this->mouse_position.y};
            this->mouse_position = {x,y};
            // TODO Maybe do acceleration?
        }

        if (e.type == SDL_MOUSEWHEEL) {
            // TODO: Handle   
        }


        switch (e.type) {
            case SDL_MOUSEBUTTONDOWN: {
                printf("%i", e.button.button);
                this->mouse_clicked_once[this->mouse_map[e.button.button]] = true;
                this->mouse_button_held[this->mouse_map[e.button.button]] = true;
                break;
            }
            case SDL_MOUSEBUTTONUP:
                this->mouse_button_held[this->mouse_map[e.button.button]] = false;
                break;
        }

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) quit();

        switch (this->state) {
            case Listening:
                switch (e.type) {                    
                    case SDL_KEYDOWN: {
                        SDL_Keycode key = e.key.keysym.sym;
                        if (e.key.repeat == 0) this->keys_pressed_once[key_map[key]] = true;
                        this->keys_held_down[key_map[key]] = true;
                        break;
                    }
                    case SDL_KEYUP: {
                        SDL_Keycode key = e.key.keysym.sym;
                        this->keys_held_down[key_map[key]] = false;
                        break;
                    }
                }
                break;
            case Rebinding:
                switch (e.type) {
                    case SDL_KEYUP:
                        this->state = Listening;
                    case SDL_KEYDOWN:
                        if (!this->rebind_finished) this->bind_key(e.key.keysym.sym);
                        this->rebind_finished = true;
                        break;
                }
                break;
            default:
                std::cout << "Unhandled case: " << this->state << std::endl;
                break;
        }
    }
}

const std::string useraction_to_name(UserAction a) {
    switch (a) {
        case MoveLeft:
            return "MoveLeft";
        case MoveRight:
            return "MoveRight";
        case Jump:
            return "Jump";
        case Attack:
            return "Attack";
        default:
            return "Not mapped";
    }
}
