#pragma once
#include <map>
#include <iostream>
#include <SDL2/SDL.h>

enum InputState {
    Listening,
    Rebinding
};

enum RebindingDevice {
    KeyboardAndMouse,
    Controller
};

enum UserAction {
    MoveLeft = 1,
    MoveRight,
    Jump,
    Attack,
    Select
};

struct MouseButtons {
    bool left_mouse_button = false;
    bool right_mouse_button = false;
    bool middle_mouse_button = false;
};

typedef std::map<SDL_Keycode, UserAction> KeyMap;
typedef std::map<UserAction, bool> KeyPresses;

typedef std::map<Uint8, UserAction> MouseMap;
typedef std::map<UserAction, bool> MousePresses;

typedef std::map< Uint8, UserAction> ControllerMap;
typedef std::map<UserAction, bool> ControllerPresses;

const std::string useraction_to_name(UserAction a);

class Input {
    private:
        bool rebind_finished = false;
        UserAction rebind_action; // TODO I guess this should be public?
        SDL_Keycode key_to_replace = SDLK_UNKNOWN;
        Uint8 mouse_button_to_replace = SDLK_UNKNOWN;
        Uint8 controller_button_to_replace = SDL_CONTROLLER_BUTTON_INVALID;
    public:
        SDL_Point mouse_position{0, 0};
        SDL_Point mouse_delta{0, 0};
        SDL_Point mouse_wheel{0, 0};
        bool handle_input();
        // Find better names
        MouseButtons mouse_clicked;
        MouseButtons mouse_held;
        // End
        InputState state = InputState::Listening;
        RebindingDevice rebinding_device = RebindingDevice::KeyboardAndMouse;
        void set_action_to_rebind(UserAction, RebindingDevice, SDL_Keycode);
        void set_action_to_rebind(UserAction, RebindingDevice, Uint8);
        KeyMap &key_map;
        KeyPresses keys_held_down;
        KeyPresses keys_pressed_once;
        MouseMap &mouse_map;
        MousePresses mouse_clicked_once;
        MousePresses mouse_button_held;
        ControllerMap &controller_map;
        ControllerPresses controller_held_down;
        ControllerPresses controller_pressed_once; 
        bool is_down(UserAction);
        bool is_pressed_once(UserAction);
        Input(KeyMap &, MouseMap &, ControllerMap &);
        ~Input();
        void bind_key(SDL_Keycode);
        void bind_mouse_button(Uint8);
        void bind_controller_button(Uint8);
};