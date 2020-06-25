#include <SDL2/SDL.h>
#include <iostream>
#include <map>
#include "Input.hpp"

bool Input::is_down(UserAction a) {
    return this->controller_held_down[a] || this->mouse_button_held[a] || this->keys_held_down[a];
}

bool Input::is_pressed_once(UserAction a) {
    return this->controller_pressed_once[a] || this->mouse_clicked_once[a] || this->keys_pressed_once[a];
}

Input::Input(
    KeyMap &key_map,
    MouseMap &mouse_map,
    ControllerMap &controller_map
): key_map{key_map}, mouse_map{mouse_map}, controller_map{controller_map} {}

Input::~Input() {}

void Input::bind_key(SDL_Keycode key) {
    this->key_map[key] = this->rebind_action;
}

void Input::bind_mouse_button(Uint8 button) {
    std::cout << "Binding mouse" << std::endl;
    this->mouse_map[button] = this->rebind_action;
}

void Input::bind_controller_button(Uint8 button) {
    this->controller_map[button] = this->rebind_action;
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
    if (device == RebindingDevice::Controller) {
        this->controller_button_to_replace = button_to_replace;
    }
    else {
        this->mouse_button_to_replace = button_to_replace;
    }
}

/*
* Updates input based on polling SDL_Events. Returns boolean whether or not "Quit" ocurred
*/
bool Input::handle_input() {
    
    for (auto p : this->keys_pressed_once) {
        this->keys_pressed_once[p.first] = false;
    }

    for (auto p : this->controller_pressed_once) {
        this->controller_pressed_once[p.first] = false;
    }

    for (auto p : this->mouse_clicked_once) {
        this->mouse_clicked_once[p.first] = false;
    }

    this->mouse_clicked.left_mouse_button = false;
    this->mouse_clicked.middle_mouse_button = false;
    this->mouse_clicked.right_mouse_button = false;

    SDL_Event e;
    int y = 0;
    int x = 0;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) return true;
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
                        this->controller_pressed_once[this->controller_map[e.cbutton.button]] = true;
                        this->controller_held_down[this->controller_map[e.cbutton.button]] = true;
                        break;
                    }
                    case SDL_CONTROLLERBUTTONUP: {
                        this->controller_held_down[this->controller_map[e.cbutton.button]] = false;
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
                        if (this->rebinding_device == RebindingDevice::Controller && !this->rebind_finished) {
                            if (this->controller_button_to_replace != SDL_CONTROLLER_BUTTON_INVALID) {
                                this->controller_map.erase(this->controller_button_to_replace);
                                this->controller_button_to_replace = SDL_CONTROLLER_BUTTON_INVALID;
                            }
                            this->bind_controller_button(e.cbutton.button);
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
