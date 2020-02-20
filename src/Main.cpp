#include <iostream>
#include <SDL2/SDL.h>
#include <map>

#include "Input/Input.hpp"
#include "GameState.h"

using namespace std;

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
    printf("No more unused: %s\n", state);

    cout << "Finished binding keys" << endl;

    Input input(mapRef);
    GameState game_state(input);
    input.set_action_to_rebind(Jump);

    while (true) {
        game_state.input.handle_input();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}