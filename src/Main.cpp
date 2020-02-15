#include <iostream>
#include <SDL2/SDL.h>
#include <map>

enum Action
{
    MoveUp
};

bool handle_input(std::map<int, std::pair<Action, bool>> &map);

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    std::map<int, std::pair<Action, bool>> map;
    auto &mapRef = map;
    int i = 0;
    while (true) {
        if (!handle_input(mapRef))
        {
            break;
        };
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

bool handle_input(std::map<int, std::pair<Action, bool>> &map) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.key.keysym.sym) {
            case SDLK_ESCAPE: 
                if (SDL_KEYDOWN == e.type)
                    return false;
                break;
        }
    }
    return true;
}
