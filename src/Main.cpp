#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>

#include "Input/Input.hpp"
#include "Input/Input_test.hpp"
#include "Rendering/Rendering_test.hpp"
#include "Engine.h"

#if WITH_TESTS

int main(int argc, char* argv[]) {
    input_test();
    rendering_test();
    return 0;
}

#else 

int main(int argc, char* argv[]) {
    return 0;
}

#endif
