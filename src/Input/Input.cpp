#include <SDL2/SDL.h>
#include <iostream>
#include <map>
#include "Input.hpp"

bool Input::is_down(UserAction a, Sint32 controller) {
    return this->controllers[controller].controller_held_down[a] || this->mouse_button_held[a] || this->keys_held_down[a];
}

bool Input::is_pressed_once(UserAction a, Sint32 controller) {
    return this->controllers[controller].controller_pressed_once[a] || this->mouse_clicked_once[a] || this->keys_pressed_once[a];
}

Input::Input(
    KeyMap &key_map,
    MouseMap &mouse_map,
    ControllerList &controllers
): key_map{ key_map }, mouse_map{ mouse_map }, controllers{ controllers } {
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            char* mapping;
            SDL_Log("Index \'%i\' is a compatible controller, named \'%s\'", i, SDL_GameControllerNameForIndex(i));
            auto ctrl = SDL_GameControllerOpen(i);
            mapping = SDL_GameControllerMapping(ctrl);
            SDL_Log("Controller %i is mapped as \"%s\".", i, mapping);
            SDL_free(mapping);
        } else {
            SDL_Log("Index \'%i\' is not a compatible controller.", i);
        }
    }
}

Input::~Input() {}

void Input::bind_key(SDL_Keycode key) {
    this->key_map[key] = this->rebind_action;
}

void Input::bind_mouse_button(Uint8 button) {
    std::cout << "Binding mouse" << std::endl;
    this->mouse_map[button] = this->rebind_action;
}

void Input::bind_controller_button(Uint8 button, Sint32 controller) {
    this->controllers[controller].controller_map[button] = this->rebind_action;
}

void Input::set_action_to_rebind(UserAction action, RebindingDevice device, SDL_Keycode key_to_replace) {
    this->state = InputState::Rebinding;
    this->rebinding_device = device;
    this->rebind_finished = false;
    this->rebind_action = action;
    this->key_to_replace = key_to_replace;
}


void Input::set_action_to_rebind(UserAction action, RebindingDevice device, Uint8 button_to_replace) {
    this->state = InputState::Rebinding;
    this->rebinding_device = device;
    this->rebind_finished = false;
    this->rebind_action = action;
    if (device == RebindingDevice::GameController) {
        this->controller_button_to_replace = button_to_replace;
    } else {
        this->mouse_button_to_replace = button_to_replace;
    }
}

/*
* Updates input based on polling SDL_Events. Returns a boolean whether or not "Quit" ocurred
*/
bool Input::handle_input() {
    
    this->keys_pressed_once.clear();
    for (auto& cont : this->controllers) {
        cont.controller_pressed_once.clear();
    }
    this->mouse_clicked_once.clear();

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
            case SDL_CONTROLLERDEVICEADDED:
                /**< The joystick device index for the ADDED event, instance id for the REMOVED or REMAPPED event */
                std::cout << "Adding " << e.cdevice.which << std::endl;
                if (SDL_IsGameController(e.cdevice.which)) {
                    char* mapping;
                    SDL_Log("Index \'%i\' is a compatible controller, named \'%s\'", e.cdevice.which, SDL_GameControllerNameForIndex(e.cdevice.which));
                    auto ctrl = SDL_GameControllerOpen(e.cdevice.which);
                    mapping = SDL_GameControllerMapping(ctrl);
                    SDL_Log("Controller %i is mapped as \"%s\".", e.cdevice.which, mapping);
                    SDL_free(mapping);
                } else {
                    SDL_Log("Index \'%i\' is not a compatible controller.", e.cdevice.which);
                }
                continue;
            case SDL_CONTROLLERDEVICEREMOVED: {
                std::cout << "Removing" << e.cdevice.which << std::endl;
                SDL_GameControllerFromInstanceID(e.cdevice.which);
                SDL_GameControllerClose(SDL_GameControllerFromInstanceID(e.cdevice.which));
                std::cout << "Removed" << std::endl;
                continue;
            }
            case SDL_CONTROLLERDEVICEREMAPPED:
                std::cout << "Remapped" << std::endl;
                continue;
        }


        switch (this->state) {
            case Listening:
                switch (e.type) {                    
                    case SDL_KEYDOWN: {
                        SDL_Keycode key = e.key.keysym.sym;
                        if (e.key.repeat == 0) this->keys_pressed_once[key_map[key]] = true;
                        this->keys_held_down[key_map[key]] = true;
                        break;
                    }
                    case SDL_KEYUP: {
                        SDL_Keycode key = e.key.keysym.sym;
                        this->keys_pressed_once[key_map[key]] = false;
                        this->keys_held_down[key_map[key]] = false;
                        break;
                    }
                    case SDL_MOUSEBUTTONDOWN: {
                        this->mouse_clicked_once[this->mouse_map[e.button.button]] = true;
                        this->mouse_button_held[this->mouse_map[e.button.button]] = true;
                        if (e.button.button == SDL_BUTTON_LEFT) this->mouse_clicked.left_mouse_button = this->mouse_held.left_mouse_button = true;
                        else if (e.button.button == SDL_BUTTON_MIDDLE) this->mouse_clicked.middle_mouse_button = this->mouse_held.middle_mouse_button = true;
                        else if (e.button.button == SDL_BUTTON_RIGHT) this->mouse_clicked.right_mouse_button = this->mouse_held.right_mouse_button = true;
                        break;
                    }
                    case SDL_CONTROLLERAXISMOTION: {
                        const auto which_controller = e.caxis.which;
                        const auto &controller = this->controllers[which_controller];
                        switch (e.caxis.axis) {
                            case SDL_CONTROLLER_AXIS_INVALID:
                                std::cout << "Invalid" << std::endl;
                                break;
                            case SDL_CONTROLLER_AXIS_LEFTX:
                                std::cout << "LeftX" << std::endl;
                                break;
                            case SDL_CONTROLLER_AXIS_LEFTY:
                                std::cout << "LeftY" << std::endl;
                                break;
                            case SDL_CONTROLLER_AXIS_RIGHTX:
                                std::cout << "RightX" << std::endl;
                                break;
                            case SDL_CONTROLLER_AXIS_RIGHTY:
                                std::cout << "RightY" << std::endl;
                                break;
                            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                                std::cout << "TriggerLeft" << std::endl;
                                break;
                            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                                std::cout << "TriggerRight" << std::endl;
                                break;
                            case SDL_CONTROLLER_AXIS_MAX:
                                std::cout << "Axis_Max" << std::endl;
                                break;
                        }

                        break;
                    }
                    case SDL_MOUSEBUTTONUP: {
                        if (e.button.button == SDL_BUTTON_LEFT) this->mouse_held.left_mouse_button = true;
                        else if (e.button.button == SDL_BUTTON_MIDDLE) this->mouse_held.middle_mouse_button = true;
                        else if (e.button.button == SDL_BUTTON_RIGHT) this->mouse_held.right_mouse_button = true;
                        this->mouse_button_held[this->mouse_map[e.button.button]] = false;
                        break;
                    }
                    case SDL_CONTROLLERBUTTONDOWN: {
                        const Sint32 which_controller = e.cbutton.which;
                        auto &controller = this->controllers[which_controller];
                        auto action = controller.controller_map[e.cbutton.button];
                        controller.controller_pressed_once[action] = true;
                        controller.controller_held_down[action] = true;
                        break;
                    }
                    case SDL_CONTROLLERBUTTONUP: {
                        const Sint32 which_controller = e.cbutton.which;
                        std::cout << which_controller << std::endl;
                        auto &controller = this->controllers[which_controller];
                        auto action = controller.controller_map[e.cbutton.button];
                        controller.controller_pressed_once[action] = false;
                        controller.controller_held_down[action] = false;
                        break;
                    }
                    case SDL_MOUSEMOTION: {
                        const auto mouse_state = SDL_GetMouseState(&x, &y);
                        this->mouse_delta = { x - this->mouse_position.x, y - this->mouse_position.y };
                        this->mouse_position = { x,y };
                        break;
                    }
                    case SDL_MOUSEWHEEL: {
                        this->mouse_wheel = { e.wheel.x, e.wheel.y };
                        break;
                    }
                }
                break;
            case Rebinding:
                switch (e.type) {
                    case SDL_KEYDOWN:
                        if (this->rebinding_device == RebindingDevice::KeyboardAndMouse && !this->rebind_finished) {
                            if (this->key_to_replace != SDLK_UNKNOWN) {
                                this->key_map.erase(this->key_to_replace);
                                this->key_to_replace = SDLK_UNKNOWN;
                            } else if (this->mouse_button_to_replace) {
                                // TODO
                            }

                            this->bind_key(e.key.keysym.sym);
                            this->rebind_finished = true;
                            this->state = Listening;
                        }
                        break;
                    case SDL_CONTROLLERBUTTONDOWN:
                        if (this->rebinding_device == RebindingDevice::GameController && !this->rebind_finished) {
                            const auto which_controller = e.cbutton.which;
                            if (this->controller_button_to_replace != SDL_CONTROLLER_BUTTON_INVALID) {
                                this->controllers[which_controller].controller_map.erase(this->controller_button_to_replace);
                                this->controller_button_to_replace = SDL_CONTROLLER_BUTTON_INVALID;
                            }
                            this->bind_controller_button(e.cbutton.button, which_controller);
                            this->rebind_finished = true;
                            this->state = Listening;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN: 
                        if (this->rebinding_device == RebindingDevice::KeyboardAndMouse && !this->rebind_finished) {
                            if (this->key_to_replace != SDLK_UNKNOWN) {
                                this->key_map.erase(this->key_to_replace);
                                this->key_to_replace = SDLK_UNKNOWN;
                            } // else if (this->mouse_key_to_replace)
                            this->bind_mouse_button(e.button.button);
                            this->rebind_finished = true;
                            this->state = Listening;
                        }
                        break;
                }
                break;
            default:
                std::cout << "Unhandled case: " << this->state << std::endl;
                break;
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
