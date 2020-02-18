#include <SDL2/SDL.h>
#include "Input.hpp"
#include <iostream>

using namespace std;

enum Action {
    MoveLeft = 1,
    MoveRight,
    Jump,
    Attack
};

Input::Input(KeyMap &map): new map() {
    this.map = map;
}

Input::~Input() {
    // Free map?
}


void Input::bind_key(Action action) {
    SDL_Event e;

    while (true) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) exit(0); 
                    map[e.key.keysym.sym] = action;
                    cout << keypress_to_name(action) << " is bound to " << SDL_GetKeyName(e.key.keysym.sym) << endl;
            }
        }
    }
}

bool handle_input(KeyMap &map) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:    
                if (e.key.keysym.sym == SDLK_ESCAPE) return false;
                cout << "Pressed " << keypress_to_name(map[e.key.keysym.sym]) << endl;

        }
    }
    return true;
}

const string keypress_to_name(Action a) {
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

void Input::poll_for_keys() {  
    for (int keyInt = MoveLeft; keyInt != Attack + 1; keyInt++) {
        Action key = static_cast<Action>(keyInt);
        cout << "Polling for " << keypress_to_name(key) << endl;
        if (!bind_key(key, mapRef)) {
            cout << "Quitting" << endl;
            goto quit;
        }
    }
}