#include <iostream>
#include <SDL2/SDL.h>
#include <map>

using namespace std;

enum Action {
    MoveLeft = 1,
    MoveRight,
    Jump,
    Attack
};

typedef map<SDL_Keycode, Action> KeyMap;

bool handle_input(KeyMap &map);
const string keypress_to_name(Action a);
bool bind_key(Action action, KeyMap &map);

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    KeyMap map;
    auto &mapRef = map;

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Poll for keys:
    for (int keyInt = MoveLeft; keyInt != Attack + 1; keyInt++) {
        Action key = static_cast<Action>(keyInt);
        cout << "Polling for " << keypress_to_name(key) << endl;
        if (!bind_key(key, mapRef)) {
            cout << "Quitting" << endl;
            goto quit;
        }
    }

    cout << "Finished binding keys" << endl;

    while (true) {
        if (!handle_input(mapRef)) {
            break;
        }
    }

    quit:
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

bool bind_key(Action action, KeyMap &map) {
    SDL_Event e;

    while (true) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) return false; 
                    map[e.key.keysym.sym] = action;
                    cout << keypress_to_name(action) << " is bound to " << SDL_GetKeyName(e.key.keysym.sym) << endl;
                    return true;
            }
        }
    }
}

bool handle_input(KeyMap &map) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_KEYDOWN:
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