#pragma once
#include <map>
#include <iostream>
#include <SDL2/SDL.h>

enum InputState {
    Listening,
    Rebinding
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

const std::string useraction_to_name(UserAction a);

class Input {
    private:
        bool rebind_finished = false;
        UserAction rebind_action; // TODO I guess this should be public?
    public:
        SDL_Point mouse_position{0, 0};
        SDL_Point mouse_delta{0, 0};
        SDL_Point mouse_wheel{0, 0};
        bool handle_input();
        InputState state = Listening;
        void set_action_to_rebind(UserAction action);
        KeyMap &key_map;
        KeyPresses keys_held_down;
        KeyPresses keys_pressed_once;
        MouseMap &mouse_map;
        MousePresses mouse_clicked_once;
        MousePresses mouse_button_held;
        ControllerMap &controller_map;
        ControllerPresses controller_held_down;
        ControllerPresses controller_pressed_once; 
        bool is_down(UserAction action);
        bool is_pressed_once(UserAction action);
        Input(KeyMap &key_map, MouseMap &mouse_clicked_once, ControllerMap &mouse_button_held);
        ~Input();
        void bind_key(SDL_Keycode key);
        void bind_mouse_button(Uint8);
        void bind_controller_button(Uint8);
};