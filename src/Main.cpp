#include <iostream>
#include <SDL.h>

int main() {
    while (true) {
        if (!handle_input()) {
            break;
        };
    }
    return 0;
}

bool handle_input() {
    SDL_Event e;
    switch (SDL_PollEvent(&e)) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_QUIT:
            return false;
    }
    return false;
}
