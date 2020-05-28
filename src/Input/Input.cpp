#include <SDL2/SDL.h>
#include <iostream>
#include <map>
#include "Input.hpp"

// This is bad, I guess?
using namespace std;

bool Input::is_down(UserAction a) {
    return this->keys_held_down[a];
}

bool Input::is_pressed_once(UserAction a) {
    return this->keys_pressed_once[a];
}

Input::Input(KeyMap &key_map, KeyPressMap &keys_held_down, KeyPressMap &keys_pressed_once) : key_map{key_map}, keys_held_down{keys_held_down}, keys_pressed_once{keys_pressed_once} {
    key_map[SDLK_LEFT] = MoveLeft;
    key_map[SDLK_RIGHT] = MoveRight;
    key_map[SDLK_SPACE] = Jump;
    key_map[SDLK_LCTRL] = Attack;
}

Input::~Input() {}

void Input::bind_key(SDL_Keycode key) {
    this->key_map[key] = this->rebind_action;
    cout << "Rebound " << useraction_to_name(this->rebind_action) << endl;
}

/* Shouldn't be here. */
void quit() {
    cout << "Quitting" << endl;
    exit(0);
}

void Input::set_action_to_rebind(UserAction action) {
    cout << "We want to rebind " << useraction_to_name(action) << endl;
    this->state = Rebinding;
    this->rebind_finished = false;
    this->rebind_action = action;
}

void Input::handle_input() {
    this->keys_pressed_once[MoveLeft] = false;
    this->keys_pressed_once[MoveRight] = false;
    this->keys_pressed_once[Jump] = false;
    this->keys_pressed_once[Attack] = false;
    SDL_Event e;
    int y = 0;
    int x = 0;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_MOUSEMOTION) {
            const auto mouse_state = SDL_GetMouseState(&x, &y);
            this->mouse_delta = {x - this->mouse_position.x, y - this->mouse_position.y};
            this->mouse_position = {x,y};
            // Maybe do acceleration?
        }

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) quit();

        switch (this->state) {
            case Listening:
                switch (e.type) {                    
                    case SDL_KEYDOWN: {
                        SDL_Keycode key = e.key.keysym.sym;
                        if (e.key.repeat == 0) this->keys_pressed_once[key_map[key]] = true;
                        this->keys_held_down[key_map[key]] = true;
                        cout << "Pressed " << useraction_to_name(key_map[key]) << endl;
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
                cout << "Unhandled case: " << this->state << endl;
                break;
        }
    }
}

const string useraction_to_name(UserAction a) {
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
