#pragma once
#include <map>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

enum InputState {
    Listening,
    Rebinding
};

enum RebindingDevice {
    KeyboardAndMouse,
    GameController
};

typedef std::map<SDL_Keycode, bool> DirectKeyMap;
typedef std::map<Uint8, bool> DirectMouseMap;
typedef std::map<Uint8, bool> DirectControllerMap;
typedef std::map<SDL_Keycode, Sint32> KeyMap;
typedef std::map<Sint32, bool> KeyPresses;

typedef std::map<Uint8, Sint32> MouseMap;
typedef std::map<Sint32, bool> MousePresses;

typedef std::map<Uint8, Sint32> ControllerMap;
typedef std::map<Sint32, bool> ControllerPresses;


struct ControllerAnalog {
    // Left is negative, right is positive
    Sint32 horizontal_axis = 0;
    // Up is negative, down is positive
    Sint32 vertical_axis = 0;
};

struct Controller {
    bool active = false;
    bool rebinding = false;
    ControllerAnalog left = { 0, 0 };
    ControllerAnalog right = { 0, 0 };
    Sint16 left_trigger = 0;
    Sint16 right_trigger = 0;
    ControllerMap controller_map;
    ControllerPresses controller_held;
    ControllerPresses controller_pressed;
    DirectControllerMap direct_button_held;
    DirectControllerMap direct_button_pressed;
    Sint32 rebind_action = (Sint32) 0;
    Uint8 button_to_replace = 0;
    int instance_id = -1;
};

typedef std::vector<Controller> ControllerList;

class Input {
    private:
        bool rebind_finished = false;
        bool rebinding = false;
        Sint32 rebind_action; // TODO I guess this should be public?
        SDL_Keycode keyboard_key_to_replace = SDLK_UNKNOWN;
        Uint8 mouse_button_to_replace = SDLK_UNKNOWN;
        Uint8 controller_button_to_replace = SDL_CONTROLLER_BUTTON_INVALID;
    public:
        SDL_Point mouse_position{0, 0};
        SDL_Point mouse_delta{0, 0};
        SDL_Point mouse_wheel{0, 0};
        bool handle_input();
        bool escape_pressed = false;
        void start_rebind_keyboard_action(Sint32, SDL_Keycode);
        void start_rebind_mouse_action(Sint32, Uint8);
        void start_rebind_action_controller(Sint32, Sint32, Uint8);
        bool unassign_controller(int);
        int next_free_controller_slot();
        KeyMap &key_map;
        KeyPresses key_held;
        KeyPresses key_pressed;
        DirectKeyMap direct_key_held;
        DirectKeyMap direct_key_pressed;
        DirectMouseMap direct_mouse_held;
        DirectMouseMap direct_mouse_pressed;
        MouseMap &mouse_map;
        MousePresses mouse_pressed;
        MousePresses mouse_held;
        Uint8 max_controllers = 8;
        std::vector<struct Controller> &controllers;
        bool is_held(Sint32, bool = false, Sint32 = 0);
        bool is_pressed(Sint32, bool = false, Sint32 = 0);
        
        bool is_key_held(SDL_Keycode);
        bool is_key_pressed(SDL_Keycode);
        bool is_mouse_button_held(Uint8);
        bool is_mouse_button_pressed(Uint8);
        bool is_controller_held(Uint8, Uint8);
        bool is_controller_presssed(Uint8, Uint8);

        Input(KeyMap&, MouseMap&, ControllerList&);
        ~Input();
        void bind_key(SDL_Keycode);
        void bind_mouse_button(Uint8);
        void bind_controller_button(Uint8, Sint32);
};