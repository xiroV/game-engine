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
    // Is controller struct currently mapped to a connected controller
    bool active = false;
    // Is a button is being rebound
    bool rebinding = false;
    // See ControllerAnalog struct
    ControllerAnalog left = { 0, 0 };
    // See ControllerAnalog struct
    ControllerAnalog right = { 0, 0 };
    // The trigger's value if pressed
    Sint16 left_trigger = 0;
    // The triggers value if pressed
    Sint16 right_trigger = 0;
    // The map of controller buttons to their associated actions.
    ControllerMap controller_map;
    // Map of actions to whether or not is currently held
    ControllerPresses controller_held;
    // Map of actions to whether or not is pressed exactly once
    ControllerPresses controller_pressed;
    // Direct map signifiying whether a specific button is held
    DirectControllerMap direct_button_held;
    // Direct map signifiying whether a specific button is pressed exactly once.
    DirectControllerMap direct_button_pressed;
    // Action being rebound
    Sint32 rebind_action = 0;
    // Button being overwritten when rebinding, if any.
    Uint8 button_to_replace = 0;
    // Instance id used when disconnecting controller
    int instance_id = -1;
};

typedef std::vector<Controller> ControllerList;

class Input {
    private:
        // State of input handling. If rebinding is true, follow that branch.
        bool rebinding = false;
        // The action being assigned.
        Sint32 rebind_action = 0;
        // If any key should be replaced when mapping a new one, this is the one to be replaced.
        SDL_Keycode keyboard_key_to_replace = SDLK_UNKNOWN;
        // If any button should be replaced when mapping a new one, this is the one to be replaced.
        Uint8 mouse_button_to_replace = SDLK_UNKNOWN;
        // If any button should be replaced when mapping a new one, this is the one to be replaced.
        Uint8 controller_button_to_replace = SDL_CONTROLLER_BUTTON_INVALID;
        // Disconnects and unassigns the controller, setting the matching controller index to inactive.
        bool unassign_controller(int);
        // Find next availabe controller slot. Goes through the Controller list and finds an inactive one.
        int next_free_controller_slot();
    public:
        // The mouse position given as x and y coordinates.
        SDL_Point mouse_position{0, 0};
        // The positional difference between last movement of the mouse. Given as x and y difference.
        SDL_Point mouse_delta{0, 0};
        // The scroll horizontally and vertically.
        SDL_Point mouse_wheel{0, 0};
        /*
        Function to update. Should be done on each iteration.
        Handles the following SDL_Events:
            SDL_CONTROLLERDEVICEADDED
            SDL_CONTROLLERDEVICEREMOVED
            SDL_KEYDOWN
            SDL_KEYUP
            SDL_MOUSEBUTTONDOWN
            SDL_MOUSEBUTTONUP
            SDL_CONTROLLERAXISMOTION
            SDL_CONTROLLERBUTTONDOWN
            SDL_CONTROLLERBUTTONUP
            SDL_MOUSEMOTION
            SDL_MOUSEWHEEL
        */
        bool handle_input();
        // Is set to true if SDLK_ESCAPE has been pressed exactly once.
        // Can also be found in 'direct_key_held' and 'direct_key_pressed'
        bool escape_pressed = false;
        // Sets an action to be rebound. Second argument is the key to be replaced, if any.
        void start_rebind_keyboard_action(Sint32, SDL_Keycode);
        // Sets an action to be rebound. Second argument is the button to be replaced, if any.
        void start_rebind_mouse_action(Sint32, Uint8);
        // Sets an action to be rebound. Second argument is the button to be replaced, if any. Third is the controller where is should be rebound.
        void start_rebind_action_controller(Sint32, Sint32, Uint8);
        // A map of keyboard keys mapped to user actions
        KeyMap key_map;
        // A map of actions mapped to whether the actions is held down
        KeyPresses key_held;
        // A map of actions mapped to whether the actions is pressed exactly once.
        KeyPresses key_pressed;
        // A map of keyboard key mapped to whether or not it is held down
        DirectKeyMap direct_key_held;
        // A map of keyboard key mapped to whether or not it is pressed exactly once
        DirectKeyMap direct_key_pressed;
        // A map of mouse buttons mapped to whether or not it is held down
        DirectMouseMap direct_mouse_held;
        // A map of mouse buttons mapped to whether or not it is pressed exactly once
        DirectMouseMap direct_mouse_pressed;
        // A map of mouse buttons mapped to user actions
        MouseMap mouse_map;
        // A map of actions mapped to whether the actions is held down
        MousePresses mouse_pressed;
        // A map of actions mapped to whether the actions is pressed exactly once
        MousePresses mouse_held;

        // The max allowed controllers attached at any point. 
        Uint8 max_controllers = 8;
        
        // List of controllers. Content is filled lazily.
        std::vector<struct Controller> controllers;
        
        /*
        Checks if action mapping is held down.
        2nd parameter must be true to check for controller as well, where 3rd parameter is which controller index. Does not check for out of bounds.
        */
        bool is_held(Sint32, bool = false, Sint32 = 0);
        
        /*
        Checks if action mapping is pressed exactly once.
        2nd parameter must be true to check for controller as well, where 3rd parameter is which controller index. Does not check for out of bounds.
        */
        bool is_pressed(Sint32, bool = false, Sint32 = 0);
        
        /*
        Checks if SDLK_<key> is held down.
        */
        bool is_key_held(SDL_Keycode);

        /*
        Checks if SDLK_<key> is pressed exactly once.
        */
        bool is_key_pressed(SDL_Keycode);

        /*
        Checks if a specific mouse button is held down.
        E.g. SDL_BUTTON_LEFT (1), SDL_BUTTON_MIDDLE (2) or SDL_BUTTON_RIGHT (3)
        */
        bool is_mouse_button_held(Uint8);

        /*
        Checks if a specific mouse button is pressed exactly once.
        E.g. SDL_BUTTON_LEFT (1), SDL_BUTTON_MIDDLE (2) or SDL_BUTTON_RIGHT (3)
        */
        bool is_mouse_button_pressed(Uint8);

        /*
        Checks if a specific controller button is held.
        E.g. SDL_CONTROLLER_BUTTON_A
        */
        bool is_controller_held(Uint8, Uint8);

        /*
        Checks if a specific controller button is pressed exactly once.
        E.g. SDL_CONTROLLER_BUTTON_A
        */
        bool is_controller_presssed(Uint8, Uint8);

        // Binds a keyboard key to an action
        void bind_key_to_action(SDL_Keycode, Sint32);

        // Binds a mouse button to an action
        void bind_mouse_button_to_action(Uint8, Sint32);

        // Binds a controller button to an action, for a specific controller
        void bind_controller_button_to_action(Uint8, Sint32, Sint32);


        Input();
        ~Input();
        void bind_key(SDL_Keycode);
        void bind_mouse_button(Uint8);
        void bind_controller_button(Uint8, Sint32);
};