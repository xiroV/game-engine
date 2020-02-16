#include <iostream>
#include <SDL2/SDL.h>
#include <map>

using namespace std;

enum Action {
    MoveLeft,
    MoveRight,
    Jump,
    Attack
};

typedef map<SDL_Keycode, Action> KeyMap;

void handle_keypress(SDL_Keycode key);
bool handle_input(KeyMap &map);
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
        cout << "Polling for " << key << endl;
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
                    cout << action << " is bound to " << e.key.keysym.sym << endl;
                    return true;
            }
        }
    }
}

bool handle_input(KeyMap &map) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            switch (e.key.keysym.sym) {
            case SDL_KEYDOWN:
                if (SDLK_ESCAPE == e.key.keysym.sym)
                    return false;
            case SDL_KEYUP:
                handle_keypress(e.key.keysym.sym);
                break;
            }
        }
    }
    return true;
}

void handle_keypress(int i) {
    return;
}