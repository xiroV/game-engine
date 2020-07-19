#include <SDL2/SDL.h>
#include <iostream>
#include <map>
#include "Input.hpp"

bool Input::is_down(UserAction a, bool with_controller, Sint32 controller) {
    const bool controller_down = with_controller ? this->controllers[controller].controller_pressed_once[a] : false;
    return controller_down || this->mouse_button_held[a] || this->keys_held_down[a];
}

bool Input::is_pressed_once(UserAction a, bool with_controller,  Sint32 controller) {
    const bool controller_down = with_controller ? this->controllers[controller].controller_pressed_once[a] : false;
    if (with_controller) this->controllers[controller].controller_pressed_once[a] = false;
    const bool mouse_down = this->mouse_clicked_once[a];
    const bool key_down = this->keys_pressed_once[a];
    this->mouse_clicked_once[a] = this->keys_pressed_once[a] = false;
    return controller_down || mouse_down || key_down;
}

int Input::next_free_controller_slot() {
    std::cout << "Controller size " << this->controllers.size() <<  std::endl;
    for (size_t i = 0; i < this->max_controllers; i++) {
        std::cout << "The next controller will be at index " << i << std::endl;
        if (i >= this->controllers.size()) {
            ControllerMap controller_map;
            Controller c = {
                false,
                false,
                {0, 0},
                {0, 0},
                controller_map
            };
            this->controllers.emplace_back(c);
            std::cout << "Added controller to list" << i << std::endl;
            return i;
        }

        if (!this->controllers[i].active) {
            std::cout << "Found inactive one." << std::endl;
            return i;
        }
    }
    return -1;
}

Input::Input(
    KeyMap &key_map,
    MouseMap &mouse_map,
    ControllerList &controllers
): key_map{ key_map }, mouse_map{ mouse_map }, controllers{ controllers } {}


void assign_controller(int which, int assign_slot) {
    char* mapping;
    std::cout << "which: " << which << ", assign slot " << assign_slot << std::endl;
    SDL_Log("Index \'%i\' is a compatible controller, named \'%s\'", which, SDL_GameControllerNameForIndex(0));
    SDL_GameController *c = SDL_GameControllerOpen(which);
    mapping = SDL_GameControllerMapping(c);
    SDL_GameControllerSetPlayerIndex(c, assign_slot);
    std::cout << SDL_GameControllerGetPlayerIndex(c) << std::endl;
    SDL_Log("Controller %i is mapped as \"%s\".", which, mapping);
    SDL_free(mapping);
}

bool Input::unassign_controller(int which) {
    SDL_GameController *controller = SDL_GameControllerFromInstanceID(which);
    const int index = SDL_GameControllerGetPlayerIndex(controller);
    this->controllers[index].active = false;
    SDL_GameControllerClose(controller);
    return false;
}

Input::~Input() {}

void Input::bind_key(SDL_Keycode key) {
    this->key_map[key] = this->rebind_action;
}

void Input::bind_mouse_button(Uint8 button) {
    this->mouse_map[button] = this->rebind_action;
}

void Input::bind_controller_button(Uint8 button, Sint32 controller) {
    this->controllers[controller].controller_map[button] = this->rebind_action;
}

void Input::start_rebind_keyboard_action(UserAction action, SDL_Keycode key_to_replace) {
    this->rebinding_keyboard = true;
    this->rebind_action = action;
    this->keyboard_key_to_replace = key_to_replace;
}

void Input::start_rebind_mouse_action(UserAction action, Uint8 button_to_replace) {
    this->rebinding_mouse = true;
    this->rebind_action = action;
    this->mouse_button_to_replace = button_to_replace;
}

void Input::start_rebind_action_controller(UserAction action, Sint32 controller, Uint8 button_to_replace) {
    this->controllers[controller].rebinding = true;
    this->controllers[controller].rebind_action = action;
    this->controllers[controller].button_to_replace = button_to_replace;
}

/**
* Updates input based on polling SDL_Events. Returns a boolean whether or not "Quit" ocurred
*/
bool Input::handle_input() {
    this->mouse_clicked.left_mouse_button = false;
    this->mouse_clicked.middle_mouse_button = false;
    this->mouse_clicked.right_mouse_button = false;
    this->escape_pressed = false;

    SDL_Event e;
    int y = 0;
    int x = 0;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) this->escape_pressed = true;
        if (e.type == SDL_QUIT) return true;

        switch (e.type) {
            case SDL_CONTROLLERDEVICEADDED: {
                const int player = this->next_free_controller_slot();
                assign_controller(e.cdevice.which, player);
                this->controllers[player].active = true;
                continue;
            }
            case SDL_CONTROLLERDEVICEREMOVED: {
                unassign_controller(e.cdevice.which);
                continue;
            }
            case SDL_CONTROLLERDEVICEREMAPPED:
                std::cout << "Remapped" << std::endl;
                continue;
            case SDL_KEYDOWN: {
                SDL_Keycode key = e.key.keysym.sym;
                if (this->rebinding_keyboard || this->rebinding_mouse) {
                    this->bind_key(key);
                    if (this->keyboard_key_to_replace != SDLK_UNKNOWN) {
                        this->key_map.erase(this->keyboard_key_to_replace);
                        this->keyboard_key_to_replace = SDLK_UNKNOWN;
                        this->rebinding_keyboard = rebinding_keyboard;
                    } else if (this->mouse_button_to_replace != SDLK_UNKNOWN) {
                        this->mouse_map.erase(this->mouse_button_to_replace);
                        this->mouse_button_to_replace = SDLK_UNKNOWN;
                        this->rebinding_mouse = false;
                    }
                } else {
                    if (e.key.repeat == 0) this->keys_pressed_once[key_map[key]] = true;
                    this->keys_held_down[key_map[key]] = true;
                }
                continue;
            }
            case SDL_KEYUP: {
                SDL_Keycode key = e.key.keysym.sym;
                this->keys_pressed_once[key_map[key]] = false;
                this->keys_held_down[key_map[key]] = false;
                continue;
            }
            case SDL_MOUSEBUTTONDOWN: {
                const Uint8 button = e.button.button;
                if (this->rebinding_mouse || this->rebinding_keyboard) {
                    this->bind_mouse_button(button);
                    if (this->keyboard_key_to_replace != SDLK_UNKNOWN) {
                        this->key_map.erase(this->keyboard_key_to_replace);
                        this->keyboard_key_to_replace = SDLK_UNKNOWN;
                        this->rebinding_keyboard = false;
                    } else if (this->mouse_button_to_replace != SDLK_UNKNOWN) {
                        this->mouse_map.erase(this->mouse_button_to_replace);
                        this->mouse_button_to_replace = SDLK_UNKNOWN;
                        this->rebinding_mouse = false;
                    }
                } else {
                    this->mouse_clicked_once[this->mouse_map[e.button.button]] = true;
                    this->mouse_button_held[this->mouse_map[e.button.button]] = true;
                    if (e.button.button == SDL_BUTTON_LEFT) this->mouse_clicked.left_mouse_button = this->mouse_held.left_mouse_button = true;
                    else if (e.button.button == SDL_BUTTON_MIDDLE) this->mouse_clicked.middle_mouse_button = this->mouse_held.middle_mouse_button = true;
                    else if (e.button.button == SDL_BUTTON_RIGHT) this->mouse_clicked.right_mouse_button = this->mouse_held.right_mouse_button = true;
                }
                continue;
            }
            case SDL_CONTROLLERAXISMOTION: {
                // const auto which_controller = e.caxis.which;
                // const auto &controller = this->controllers[which_controller];
                std::cout << e.caxis.which << std::endl;
                switch (e.caxis.axis) {
                    case SDL_CONTROLLER_AXIS_INVALID:
                        std::cout << "Invalid" << std::endl;
                        continue;
                    case SDL_CONTROLLER_AXIS_LEFTX:
                        std::cout << "LeftX" << std::endl;
                        continue;
                    case SDL_CONTROLLER_AXIS_LEFTY:
                        std::cout << "LeftY" << std::endl;
                        continue;
                    case SDL_CONTROLLER_AXIS_RIGHTX:
                        std::cout << "RightX" << std::endl;
                        continue;
                    case SDL_CONTROLLER_AXIS_RIGHTY:
                        std::cout << "RightY" << std::endl;
                        continue;
                    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                        std::cout << "TriggerLeft" << std::endl;
                        continue;
                    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                        std::cout << "TriggerRight" << std::endl;
                        continue;
                    case SDL_CONTROLLER_AXIS_MAX:
                        std::cout << "Axis_Max" << std::endl;
                        continue;
                }
                continue;
            }
            case SDL_MOUSEBUTTONUP: {
                if (e.button.button == SDL_BUTTON_LEFT) this->mouse_held.left_mouse_button = true;
                else if (e.button.button == SDL_BUTTON_MIDDLE) this->mouse_held.middle_mouse_button = true;
                else if (e.button.button == SDL_BUTTON_RIGHT) this->mouse_held.right_mouse_button = true;
                this->mouse_button_held[this->mouse_map[e.button.button]] = false;
                continue;
            }
            case SDL_CONTROLLERBUTTONDOWN: {
                const Sint32 player = SDL_GameControllerGetPlayerIndex(SDL_GameControllerFromInstanceID(e.cbutton.which));
                auto &controller = this->controllers[player];
                auto action = controller.controller_map[e.cbutton.button];
                std::cout << action << std::endl;

                controller.controller_pressed_once[action] = true;
                controller.controller_held_down[action] = true;
                continue;
            }
            case SDL_CONTROLLERBUTTONUP: {
                const Sint32 which_controller = SDL_GameControllerGetPlayerIndex(SDL_GameControllerFromInstanceID(e.cbutton.which));
                std::cout << which_controller << std::endl;
                auto &controller = this->controllers[which_controller];
                auto action = controller.controller_map[e.cbutton.button];
                controller.controller_pressed_once[action] = false;
                controller.controller_held_down[action] = false;
                continue;
            }
            case SDL_MOUSEMOTION: {
                const auto mouse_state = SDL_GetMouseState(&x, &y);
                this->mouse_delta = { x - this->mouse_position.x, y - this->mouse_position.y };
                this->mouse_position = { x,y };
                continue;
            }
            case SDL_MOUSEWHEEL: {
                this->mouse_wheel = { e.wheel.x, e.wheel.y };
                continue;
            }
        }
    }
    return false;
}

const std::string useraction_to_name(UserAction a) {
    switch (a) {
        case UserAction::MoveLeft:
            return "MoveLeft";
        case UserAction::MoveRight:
            return "MoveRight";
        case UserAction::Jump:
            return "Jump";
        case UserAction::Attack:
            return "Attack";
        default:
            return "Not mapped";
    }
}
