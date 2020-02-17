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