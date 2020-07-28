#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <map>
#include "Input.hpp"

bool Input::is_held(Sint32 a, bool with_controller, Sint32 controller) {
    const bool controller_down = with_controller ? this->controllers[controller].controller_held[a] : false;
    return controller_down || this->mouse_held[a] || this->key_held[a];
}

bool Input::is_pressed(Sint32 a, bool with_controller,  Sint32 controller) {
    const bool controller_down = with_controller ? this->controllers[controller].controller_pressed[a] : false;
    if (with_controller) this->controllers[controller].controller_pressed[a] = false;
    const bool mouse_down = this->mouse_pressed[a];
    const bool key_down = this->key_pressed[a];
    this->mouse_pressed[a] = this->key_pressed[a] = false;
    return controller_down || mouse_down || key_down;
}

bool Input::is_key_held(SDL_Keycode key) {
    return this->direct_key_held[key];
}

bool Input::is_key_pressed(SDL_Keycode key) {
    auto pressed = this->direct_key_pressed[key];
    this->direct_key_pressed[key] = false;
    return pressed;
}

bool Input::is_mouse_button_held(Uint8 button) {
    return this->direct_mouse_held[button];
}

bool Input::is_mouse_button_pressed(Uint8 button) {
    const auto is_pressed = this->direct_mouse_pressed[button];
    this->direct_mouse_pressed[button] = false;
    return is_pressed;
}

bool Input::is_controller_presssed(Uint8 button, Uint8 controller) {
    auto pressed = this->controllers[controller].direct_button_pressed[button];
    this->controllers[controller].direct_button_pressed[button] = false;
    return pressed;
}

bool Input::is_controller_held(Uint8 button, Uint8 controller) {
    return this->controllers[controller].direct_button_held[button];
}

int Input::next_free_controller_slot() {
    for (int i = 0; i < this->max_controllers; i++) {
        if (i >= this->controllers.size()) {
            ControllerMap controller_map;
            Controller c = {
                false,
                false,
                {0, 0},
                {0, 0},
                0,
                0,
                controller_map
            };
            this->controllers.emplace_back(c);
            return i;
        }

        if (!this->controllers[i].active) {
            return i;
        }
    }
    return -1;
}

Input::Input() {
    this->controllers.emplace_back(Controller{
        false,
        false,
        {0, 0},
        {0, 0},
        0,
        0,
        std::map<Uint8, Sint32>()
    });
}


int assign_controller(int which, int assign_slot) {
    char* mapping;
    SDL_Log("Index \'%i\' is a compatible controller, named \'%s\'", which, SDL_GameControllerNameForIndex(which));
    SDL_GameController *c = SDL_GameControllerOpen(which);
    mapping = SDL_GameControllerMapping(c);
    SDL_GameControllerSetPlayerIndex(c, assign_slot);
    SDL_Log("Controller %i is mapped as \"%s\".", which, mapping);
    SDL_free(mapping);
    return SDL_JoystickInstanceID(SDL_JoystickFromPlayerIndex(assign_slot));
}

bool Input::unassign_controller(int which) {
    SDL_GameController *controller = SDL_GameControllerFromInstanceID(which);
    if (controller == nullptr) return true;
    std::cout << "pre find. inde" << std::endl;
    for (auto& c : this->controllers) {
        if (c.instance_id == which) {
            c.active = false;
            c.instance_id = -1;
            break;
        }
    }
    SDL_GameControllerClose(controller);
    return false;
}

Input::~Input() {}

void Input::bind_key_to_action(SDL_Keycode key, Sint32 action) {
    this->key_map[key] = action;
}

void Input::bind_mouse_button_to_action(Uint8 button, Sint32 action) {
    this->mouse_map[button] = action;
}

void Input::bind_controller_button_to_action(Uint8 button, Sint32 action, Sint32 controller) {
    this->controllers[controller].controller_map[button] = action;
}

void Input::bind_key(SDL_Keycode key) {
    this->key_map[key] = this->rebind_action;
}

void Input::bind_mouse_button(Uint8 button) {
    this->mouse_map[button] = this->rebind_action;
}

void Input::bind_controller_button(Uint8 button, Sint32 controller) {
    this->controllers[controller].controller_map[button] = this->rebind_action;
}

void Input::start_rebind_keyboard_action(Sint32 action, SDL_Keycode key_to_replace) {
    this->rebinding = true;
    this->rebind_action = action;
    this->keyboard_key_to_replace = key_to_replace;
}

void Input::start_rebind_mouse_action(Sint32 action, Uint8 button_to_replace) {
    this->rebinding = true;
    this->rebind_action = action;
    this->mouse_button_to_replace = button_to_replace;
}

void Input::start_rebind_action_controller(Sint32 action, Sint32 controller, Uint8 button_to_replace) {
    this->controllers[controller].rebinding = true;
    this->controllers[controller].rebind_action = action;
    this->controllers[controller].button_to_replace = button_to_replace;
}

/**
* Updates input based on polling SDL_Events. Returns a boolean whether or not "Quit" ocurred
*/
bool Input::handle_input() {
    this->escape_pressed = false;

    SDL_Event e;
    int y = 0;
    int x = 0;
    this->mouse_wheel = { 0, 0 };
    this->mouse_delta = { 0, 0 };
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) this->escape_pressed = true;
        if (e.type == SDL_QUIT) return true;

        switch (e.type) {
            case SDL_CONTROLLERDEVICEADDED: {
                const int player = this->next_free_controller_slot();
                const int instance_id = assign_controller(e.cdevice.which, player);
                this->controllers[player].active = true;
                this->controllers[player].instance_id = instance_id;
                continue;
            }
            case SDL_CONTROLLERDEVICEREMOVED: {
                unassign_controller(e.cdevice.which);
                continue;
            }
            case SDL_CONTROLLERDEVICEREMAPPED:
                std::cout << "Remapped, unhandled" << std::endl;
                continue;
            case SDL_KEYDOWN: {
                SDL_Keycode key = e.key.keysym.sym;
                if (this->rebinding) {
                    if (this->keyboard_key_to_replace != SDLK_UNKNOWN) {
                        this->key_map.erase(this->keyboard_key_to_replace);
                        this->keyboard_key_to_replace = SDLK_UNKNOWN;
                    } else if (this->mouse_button_to_replace != SDLK_UNKNOWN) {
                        this->mouse_map.erase(this->mouse_button_to_replace);
                        this->mouse_button_to_replace = SDLK_UNKNOWN;
                    }
                    this->bind_key(key);
                    this->rebinding = false;
                } else {
                    if (e.key.repeat == 0) {
                        this->key_pressed[key_map[key]] = true;
                        this->direct_key_pressed[key] = true;
                    }
                    this->key_held[key_map[key]] = true;
                    this->direct_key_held[key] = true;
                }
                continue;
            }
            case SDL_KEYUP: {
                SDL_Keycode key = e.key.keysym.sym;
                const auto action = key_map[key];
                this->key_pressed[action] = false;
                this->key_held[action] = false;
                this->direct_key_pressed[key] = false;
                this->direct_key_held[key] = false;
                continue;
            }
            case SDL_MOUSEBUTTONDOWN: {
                const Uint8 button = e.button.button;
                if (this->rebinding) {
                    if (this->keyboard_key_to_replace != SDLK_UNKNOWN) {
                        this->key_map.erase(this->keyboard_key_to_replace);
                        this->keyboard_key_to_replace = SDLK_UNKNOWN;
                    } else if (this->mouse_button_to_replace != SDLK_UNKNOWN) {
                        this->mouse_map.erase(this->mouse_button_to_replace);
                        this->mouse_button_to_replace = SDLK_UNKNOWN;
                    }
                    this->bind_mouse_button(button);
                    this->rebinding = false;
                } else {
                    const auto action = this->mouse_map[button];
                    this->mouse_held[action] = true;
                    this->mouse_pressed[action] = true;
                    this->direct_mouse_held[button] = true;
                    this->direct_mouse_pressed[button] = true;
                }
                continue;
            }
            case SDL_MOUSEBUTTONUP: {
                const auto button = e.button.button;
                const auto action = this->mouse_map[button];
                this->mouse_held[action] = false;
                this->mouse_pressed[action] = false;
                this->direct_mouse_held[button] = false;
                this->direct_mouse_pressed[button] = false;
                continue;
            }
            case SDL_CONTROLLERAXISMOTION: {
                const auto which_controller = e.caxis.which;
                const int player_index = SDL_GameControllerGetPlayerIndex(SDL_GameControllerFromInstanceID(e.caxis.which));
                Controller &c = this->controllers[player_index];
                switch (e.caxis.axis) {
                    case SDL_CONTROLLER_AXIS_INVALID:
                        continue;
                    case SDL_CONTROLLER_AXIS_LEFTX:
                        c.left.horizontal_axis = e.caxis.value;
                        continue;
                    case SDL_CONTROLLER_AXIS_LEFTY:
                        c.left.vertical_axis = e.caxis.value;
                        continue;
                    case SDL_CONTROLLER_AXIS_RIGHTX:
                        c.right.horizontal_axis = e.caxis.value;
                        continue;
                    case SDL_CONTROLLER_AXIS_RIGHTY:
                        c.right.vertical_axis = e.caxis.value;
                        continue;
                    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                        c.left_trigger = e.caxis.value;
                        continue;
                    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                        c.right_trigger = e.caxis.value;
                        continue;
                    case SDL_CONTROLLER_AXIS_MAX:
                        std::cout << "Axis_Max" << std::endl;
                        continue;
                }
                continue;
            }
            case SDL_CONTROLLERBUTTONDOWN: {
                const Sint32 player = SDL_GameControllerGetPlayerIndex(SDL_GameControllerFromInstanceID(e.cbutton.which));
                auto &controller = this->controllers[player];
                const Uint8 button = e.cbutton.button;
                if (controller.rebinding) {
                    if (controller.button_to_replace != SDL_CONTROLLER_BUTTON_INVALID) {
                        controller.controller_map.erase(controller.button_to_replace);
                        controller.button_to_replace = SDL_CONTROLLER_BUTTON_INVALID;
                    }
                    controller.controller_map[button] = controller.rebind_action;
                    controller.rebinding = false;
                } else {
                    auto action = controller.controller_map[e.cbutton.button];
                    controller.direct_button_held[e.cbutton.button] = true;
                    controller.direct_button_pressed[e.cbutton.button] = true;
                    controller.controller_pressed[action] = true;
                    controller.controller_held[action] = true;
                }
                continue;
            }
            case SDL_CONTROLLERBUTTONUP: {
                const Sint32 which_controller = SDL_GameControllerGetPlayerIndex(SDL_GameControllerFromInstanceID(e.cbutton.which));
                auto &controller = this->controllers[which_controller];
                auto action = controller.controller_map[e.cbutton.button];
                controller.direct_button_held[e.cbutton.button] = false;
                controller.direct_button_pressed[e.cbutton.button] = false;
                controller.controller_pressed[action] = false;
                controller.controller_held[action] = false;
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