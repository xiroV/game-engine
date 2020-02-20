#include <SDL2/SDL.h>
#include <iostream>
#include <map>
#include "Input.hpp"

using namespace std;

Input::Input(KeyMap &map) : map{map} {
    map[SDLK_LEFT] = MoveLeft;
    map[SDLK_RIGHT] = MoveRight;
    map[SDLK_SPACE] = Jump;
    map[SDLK_LCTRL] = Attack;
}

Input::~Input() {}


void Input::bind_key(SDL_Keycode key) {
    this->map[key] = this->rebind_action;
    cout << "Rebound " << keypress_to_name(this->rebind_action) << endl;
}

/* Shouldn't be here. */
void quit() {
    cout << "Quitting" << endl;
    exit(0);
}

void Input::set_action_to_rebind(UserAction action) {
    cout << "We want to rebind " << keypress_to_name(action) << endl;
    this->state = Rebinding;
    this->rebind_finished = false;
    this->rebind_action = action;
}

void Input::handle_input() {
    SDL_Event e;
    int y = 0;
    int x = 0;
    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_MOUSEMOTION) {
            SDL_GetMouseState(&x, &y);
            this->mouse_delta = {this->mouse_position.x - x, this->mouse_position.y - y};
            this->mouse_position = {x,y};

            // Maybe do acceleration?
        }

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) quit();

        switch (this->state) {
            case Listening:
                switch (e.type) {
                    case SDL_KEYDOWN:
                        cout << "Pressed " << keypress_to_name(map[e.key.keysym.sym]) << endl;
                        break;
                    case SDL_KEYUP:
                        break;
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

const string keypress_to_name(UserAction a) {
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
