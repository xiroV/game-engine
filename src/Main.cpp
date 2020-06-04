#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>

#include "Input/Input.hpp"
#include "Input/Input_test.hpp"
#include "Engine.h"

using namespace std;

#if WITH_TESTS

int main(int argc, char const *argv[]) {
    bool any_test = false;
    if (argc > 1) {
        // Skip run command

        for (int i = 1; i < argc; i++) {
            if (string("input-test").compare(argv[i]) == 0) {
                input_test();
                any_test = true;
            }
        }
    }

    if (!any_test) {
        cout << "No test was run" << endl;
    }
    return 0;
}

#else 

int main() {
    if (TTF_Init() < 0) {
        std::cout << "Failed to initialize TTF" << std::endl;
    }   
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    printf("No more unused: %s\n", state);

    cout << "Finished binding keys" << endl;
    
    KeyMap keys;
    MouseMap mouse;
    ControllerMap controller;
    Input input(keys, mouse, controller);
    Engine engine(input);
    engine.input.set_action_to_rebind(Jump);

    while (true) {
        engine.input.handle_input();
    }

    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#endif