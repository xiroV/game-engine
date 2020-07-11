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

enum UserAction {
    MoveLeft = 1,
    MoveRight,
    Jump,
    Attack,
    Select
};

typedef std::map<SDL_Keycode, UserAction> KeyMap;
typedef std::map<UserAction, bool> KeyPresses;

typedef std::map<Uint8, UserAction> MouseMap;
typedef std::map<UserAction, bool> MousePresses;

typedef std::map<Uint8, UserAction> ControllerMap;
typedef std::map<UserAction, bool> ControllerPresses;


struct ControllerAnalog {
    // Left is positive, right is negative
    double horizontal_axis = 0;
    // Up is positive, down is negative
    double vertical_axis = 0;
};

struct Controller {
    bool active = false;
    bool rebinding = false;
    ControllerAnalog left = { 0, 0 };
    ControllerAnalog right = { 0, 0 };
    ControllerMap controller_map;
    ControllerPresses controller_held_down;
    ControllerPresses controller_pressed_once;
    UserAction rebind_action = (UserAction) 0;
    Uint8 button_to_replace = 0;
};

typedef std::vector<Controller> ControllerList;

// Fixed mouse buttons. Contains Left, Right and Middle mouse buttons.
struct MouseButtons {
    bool left_mouse_button = false;
    bool right_mouse_button = false;
    bool middle_mouse_button = false;
};

const std::string useraction_to_name(UserAction a);

class Input {
    private:
        bool rebind_finished = false;
        bool rebinding_keyboard = false;
        bool rebinding_mouse = false;
        UserAction rebind_action; // TODO I guess this should be public?
        SDL_Keycode keyboard_key_to_replace = SDLK_UNKNOWN;
        Uint8 mouse_button_to_replace = SDLK_UNKNOWN;
        Uint8 controller_button_to_replace = SDL_CONTROLLER_BUTTON_INVALID;
    public:
        SDL_Point mouse_position{0, 0};
        SDL_Point mouse_delta{0, 0};
        SDL_Point mouse_wheel{0, 0};
        bool handle_input();
        bool escape_pressed = false;
        // TODO Find better names
        MouseButtons mouse_clicked;
        MouseButtons mouse_held;
        // TODO End
        void start_rebind_keyboard_action(UserAction, SDL_Keycode);
        void start_rebind_mouse_action(UserAction, Uint8);
        void start_rebind_action_controller(UserAction, Sint32, Uint8);
        bool unassign_controller(int);
        int next_free_controller_slot();
        KeyMap &key_map;
        KeyPresses keys_held_down;
        KeyPresses keys_pressed_once;
        MouseMap &mouse_map;
        MousePresses mouse_clicked_once;
        MousePresses mouse_button_held;
        Uint8 max_controllers = 8;
        std::vector<struct Controller> &controllers;
        bool is_down(UserAction, bool = false, Sint32 = 0);
        bool is_pressed_once(UserAction, bool = false, Sint32 = 0);
        Input(KeyMap&, MouseMap&, ControllerList&);
        ~Input();
        void bind_key(SDL_Keycode);
        void bind_mouse_button(Uint8);
        void bind_controller_button(Uint8, Sint32);
};