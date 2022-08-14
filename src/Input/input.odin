package input

import "core:fmt"
import sdl "vendor:sdl2"

InputState :: enum {
    Listening,
    Rebinding,
}

RebindingDevice :: enum {
    KeyboardAndMouse,
    GameController,
}

ControllerList :: [dynamic]^Controller
DirectKeyMap :: map[sdl.Keycode]bool
DirectMouseMap :: map[u8]bool
DirectControllerMap :: map[u8]bool
KeyMap :: map[sdl.Keycode]i32
KeyPresses :: map[i32]bool

MouseMap :: map[u8]i32
MousePresses :: map[i32]bool

ControllerMap :: map[sdl.GameControllerButton]i32
ControllerPresses :: map[i32]bool


ControllerAnalog :: struct {
    // Left is negative, right is positive
    horizontal_axis: i16,
    // Up is negative, down is positive
    vertical_axis: i16,
}

initController :: proc() -> ^Controller {
    controller := new(Controller)
    controller.instance_id = -1
    controller.controller_map = new(ControllerMap)
    controller.controller_held = new(ControllerPresses) 
    controller.controller_pressed = new(ControllerPresses) 
    controller.direct_button_held = new(DirectControllerMap) 
    controller.direct_button_pressed = new(DirectControllerMap) 
    return controller
}

Controller :: struct {
    // Is controller struct currently mapped to a connected controller
    active: bool,
    // Is a button is being rebound
    rebinding: bool,
    // See ControllerAnalog struct
    left: ControllerAnalog,
    // See ControllerAnalog struct
    right: ControllerAnalog,
    // The trigger's value if pressed
    left_trigger: i16,
    // The triggers value if pressed
    right_trigger: i16,
    // The map of controller buttons to their associated actions.
    controller_map: ^ControllerMap,
    // Map of actions to whether or not is currently held
    controller_held: ^ControllerPresses, 
    // Map of actions to whether or not is pressed exactly once
    controller_pressed: ^ControllerPresses, 
    // Direct map signifiying whether a specific button is held
    direct_button_held: ^DirectControllerMap, 
    // Direct map signifiying whether a specific button is pressed exactly once.
    direct_button_pressed: ^DirectControllerMap, 
    // Action being rebound
    rebind_action: i32,
    // Button being overwritten when rebinding, if any.
    button_to_replace: sdl.GameControllerButton,
    // Instance id used when disconnecting controller
    instance_id: sdl.JoystickID,
}

Input :: struct {
    // State of input handling. If rebinding is true, follow that branch.
    rebinding: bool,
    // The action being assigned.
    rebind_action: i32,
        // If any key should be replaced when mapping a new one, this is the one to be replaced.
    keyboard_key_to_replace: sdl.Keycode,
        // If any button should be replaced when mapping a new one, this is the one to be replaced.
    mouse_button_to_replace: u8,
        // If any button should be replaced when mapping a new one, this is the one to be replaced.
    controller_button_to_replace: sdl.GameControllerButton,
    // Disconnects and unassigns the controller, setting the matching controller index to inactive.
    // Find next availabe controller slot. Goes through the Controller list and finds an inactive one.
    // The mouse position given as x and y coordinates.
    mouse_position: sdl.Point,
    // The positional difference between last movement of the mouse. Given as x and y difference.
    mouse_delta: sdl.Point,
        // The scroll horizontally and vertically.
    mouse_wheel: sdl.Point,
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
    // Is set to true if SDLK_ESCAPE has been pressed exactly once.
    // Can also be found in 'direct_key_held' and 'direct_key_pressed'
    escape_pressed: bool,
    // A map of keyboard keys mapped to user actions
    key_map: ^KeyMap,
    // A map of actions mapped to whether the actions is held down
    key_held: ^KeyPresses,
    // A map of actions mapped to whether the actions is pressed exactly once.
    key_pressed: ^KeyPresses,
    // A map of keyboard key mapped to whether or not it is held down
    direct_key_held: ^DirectKeyMap,
    // A map of keyboard key mapped to whether or not it is pressed exactly once
    direct_key_pressed: ^DirectKeyMap,
    // A map of mouse buttons mapped to whether or not it is held down
    direct_mouse_held: ^DirectMouseMap,
    // A map of mouse buttons mapped to whether or not it is pressed exactly once
    direct_mouse_pressed: ^DirectMouseMap,
    // A map of mouse buttons mapped to user actions
    mouse_map: ^MouseMap,
    // A map of actions mapped to whether the actions is held down
    mouse_pressed: ^MousePresses,
    // A map of actions mapped to whether the actions is pressed exactly once
    mouse_held: ^MousePresses,
    // The max allowed controllers attached at any point. 
    max_controllers: int,
    // List of controllers. Content is filled lazily.
    controllers: ^ControllerList,
}

initInput :: proc() -> ^Input {
    input := new(Input)
    input.keyboard_key_to_replace = sdl.Keycode.UNKNOWN
    input.mouse_button_to_replace = 0
    input.controller_button_to_replace = sdl.GameControllerButton.INVALID
    input.max_controllers = 8

    input.key_map = new(KeyMap)
    input.key_held = new(KeyPresses)
    input.key_pressed = new(KeyPresses)
    input.direct_key_held = new(DirectKeyMap)
    input.direct_key_pressed = new(DirectKeyMap)
    input.direct_mouse_held = new(DirectMouseMap)
    input.direct_mouse_pressed = new(DirectMouseMap)
    input.mouse_map = new(MouseMap)
    input.mouse_pressed = new(MousePresses)
    input.mouse_held = new(MousePresses)
    input.controllers = new(ControllerList)

    append(input.controllers, initController())
    return input
}

is_held :: proc(input: ^Input, a: i32, with_controller: bool, controller: i32) -> bool {
    controller_down := (with_controller & input.controllers[controller].controller_held[a])
    return controller_down || input.mouse_held[a] || input.key_held[a]
}

is_pressed :: proc(input: ^Input, a: i32, with_controller: bool, controller: i32) -> bool {
    controller_down := (with_controller & input.controllers[controller].controller_pressed[a])
    mouse_down := input.mouse_pressed[a]
    key_down := input.key_pressed[a]
    input.mouse_pressed[a] = false
    input.key_pressed[a] = false
    return controller_down | mouse_down | key_down
}

is_key_held :: proc(input: ^Input, key: sdl.Keycode ) -> bool {
    return input.direct_key_held[key]
}

is_key_pressed :: proc(input: ^Input, key: sdl.Keycode) -> bool {
    pressed := input.direct_key_pressed[key]
    input.direct_key_pressed[key] = false
    return pressed
}

is_mouse_button_held :: proc(input: ^Input, button: u8) -> bool {
    return input.direct_mouse_held[button]
}

is_mouse_button_pressed :: proc(input: ^Input, button: u8) -> bool {
    is_pressed := input.direct_mouse_pressed[button]
    input.direct_mouse_pressed[button] = false
    return is_pressed
}

is_controller_presssed :: proc(input: ^Input, button: u8, controller: u8) -> bool {
    pressed := input.controllers[controller].direct_button_pressed[button]
    input.controllers[controller].direct_button_pressed[button] = false
    return pressed
}

is_controller_held :: proc(input: ^Input, button: u8, controller: u8) -> bool {
    return input.controllers[controller].direct_button_held[button]
}

next_free_controller_slot :: proc(input: ^Input) -> (bool, i32) {
    for i := 0; i < input.max_controllers; i += 1 {
        if i >= len(input.controllers) {
            append(input.controllers, initController())
            return true, auto_cast (i + 1)
        }

        if !input.controllers[i].active {
            return true, auto_cast i
        }
    }
    return false, 0
}

assign_controller :: proc(which: i32, assign_slot: i32) -> sdl.JoystickID {
    sdl.Log("Index \'%i\' is a compatible controller, named \'%s\'", which, sdl.GameControllerNameForIndex(which))
    c := sdl.GameControllerOpen(which)
    mapping := sdl.GameControllerMapping(c)
    sdl.GameControllerSetPlayerIndex(c, assign_slot)
    sdl.Log("Controller %i is mapped as \"%s\".", which, mapping)
    // TODO sdl.free(mapping)
    return sdl.JoystickInstanceID(sdl.JoystickFromPlayerIndex(assign_slot))
}

unassign_controller :: proc(input: ^Input, which: sdl.JoystickID) -> bool {
    controller := sdl.GameControllerFromInstanceID(which)
    
    if controller == nil do return true
    
    for c in input.controllers {
        if c.instance_id == which {
            c.active = false
            c.instance_id = -1
            break
        }
    }

    sdl.GameControllerClose(controller)
    return false
}

deinitInput :: proc(input: ^Input) {
    fmt.println("deinit input TODO")
}

bind_key_to_action :: proc(input: ^Input, key: sdl.Keycode, action: i32) {
    input.key_map[key] = action
}

bind_mouse_button_to_action :: proc(input: ^Input, button: u8, action: i32) {
    input.mouse_map[button] = action
}

bind_controller_button_to_action :: proc(input: ^Input, button: sdl.GameControllerButton, action: i32, controller: i32) {
    input.controllers[controller].controller_map[button] = action
}

bind_key :: proc(input: ^Input, key: sdl.Keycode ) {
    input.key_map[key] = input.rebind_action
}

bind_mouse_button :: proc(input: ^Input, button: u8) {
    input.mouse_map[button] = input.rebind_action
}

bind_controller_button :: proc(input: ^Input, button: sdl.GameControllerButton, controller: i32) {
    input.controllers[controller].controller_map[button] = input.rebind_action
}

start_rebind_keyboard_action :: proc(input: ^Input, action: i32, key_to_replace: sdl.Keycode) {
    input.rebinding = true
    input.rebind_action = action
    input.keyboard_key_to_replace = key_to_replace
}

start_rebind_mouse_action :: proc(input: ^Input, action: i32, button_to_replace: u8) {
    input.rebinding = true
    input.rebind_action = action
    input.mouse_button_to_replace = button_to_replace
}

start_rebind_action_controller :: proc(input: ^Input, action: i32, controller: i32, button_to_replace: sdl.GameControllerButton) {
    input.controllers[controller].rebinding = true
    input.controllers[controller].rebind_action = action
    input.controllers[controller].button_to_replace = button_to_replace
}

/**
* Updates input based on polling sdl.Events. Returns a boolean whether or not "Quit" ocurred
*/
handle_input :: proc(input: ^Input) -> bool {
    input.escape_pressed = false

    e: sdl.Event
    y :i32 = 0
    x :i32 = 0
    input.mouse_wheel = { 0, 0 }
    input.mouse_delta = { 0, 0 }
    for sdl.PollEvent(&e) {
        if e.type == sdl.EventType.KEYDOWN && e.key.keysym.sym == sdl.Keycode.ESCAPE do input.escape_pressed = true
        
        #partial switch (e.type) {
            case sdl.EventType.QUIT:
                return true
            case sdl.EventType.CONTROLLERDEVICEADDED: {
                valid, player := next_free_controller_slot(input)
                if !valid do continue
                instance_id := assign_controller(e.cdevice.which, player)
                input.controllers[player].active = true
                input.controllers[player].instance_id = auto_cast instance_id
                continue
            }
            case sdl.EventType.CONTROLLERDEVICEREMOVED: {
                unassign_controller(input, auto_cast e.cdevice.which)
                continue
            }
            case sdl.EventType.CONTROLLERDEVICEREMAPPED:
                fmt.println("Remapped, unhandled")
                continue
            case sdl.EventType.KEYDOWN: {
                key := e.key.keysym.sym
                if input.rebinding {
                    if (input.keyboard_key_to_replace != sdl.Keycode.UNKNOWN) {
                        delete_key(input.key_map, input.keyboard_key_to_replace)
                        input.keyboard_key_to_replace = sdl.Keycode.UNKNOWN
                    } else if (input.mouse_button_to_replace != 0) {
                        delete_key(input.mouse_map, input.mouse_button_to_replace)
                        input.mouse_button_to_replace = 0
                    }
                    bind_key(input, key)
                    input.rebinding = false
                } else {
                    if e.key.repeat == 0 {
                        input.key_pressed[input.key_map[key]] = true
                        input.direct_key_pressed[key] = true
                    }
                    input.key_held[input.key_map[key]] = true
                    input.direct_key_held[key] = true
                }
                continue
            }
            case sdl.EventType.KEYUP: {
                key := e.key.keysym.sym
                action := input.key_map[key]
                input.key_pressed[action] = false
                input.key_held[action] = false
                input.direct_key_pressed[key] = false
                input.direct_key_held[key] = false
                continue
            }
            case sdl.EventType.MOUSEBUTTONDOWN: {
                button := e.button.button
                if input.rebinding {
                    if input.keyboard_key_to_replace != sdl.Keycode.UNKNOWN {
                        delete_key(input.key_map, input.keyboard_key_to_replace)
                        input.keyboard_key_to_replace = sdl.Keycode.UNKNOWN
                    } else if input.mouse_button_to_replace != 0 {
                        delete_key(input.mouse_map, input.mouse_button_to_replace)
                        input.mouse_button_to_replace = 0
                    }
                    bind_mouse_button(input, button)
                    input.rebinding = false
                } else {
                    action := input.mouse_map[button]
                    input.mouse_held[action] = true
                    input.mouse_pressed[action] = true
                    input.direct_mouse_held[button] = true
                    input.direct_mouse_pressed[button] = true
                }
                continue
            }
            case sdl.EventType.MOUSEBUTTONUP: {
                button := e.button.button
                action := input.mouse_map[button]
                input.mouse_held[action] = false
                input.mouse_pressed[action] = false
                input.direct_mouse_held[button] = false
                input.direct_mouse_pressed[button] = false
                continue
            }
            case sdl.EventType.CONTROLLERAXISMOTION: {
                which_controller := e.caxis.which
                player_index := sdl.GameControllerGetPlayerIndex(sdl.GameControllerFromInstanceID(e.caxis.which))
                c := input.controllers[player_index]
                switch auto_cast e.caxis.axis {
                    case sdl.GameControllerAxis.INVALID:
                        continue
                    case sdl.GameControllerAxis.LEFTX:
                        c.left.horizontal_axis = e.caxis.value
                        continue
                    case sdl.GameControllerAxis.LEFTY:
                        c.left.vertical_axis = e.caxis.value
                        continue
                    case sdl.GameControllerAxis.RIGHTX:
                        c.right.horizontal_axis = e.caxis.value
                        continue
                    case sdl.GameControllerAxis.RIGHTY:
                        c.right.vertical_axis = e.caxis.value
                        continue
                    case sdl.GameControllerAxis.TRIGGERLEFT:
                        c.left_trigger = e.caxis.value
                        continue
                    case sdl.GameControllerAxis.TRIGGERRIGHT:
                        c.right_trigger = e.caxis.value
                        continue
                    case sdl.GameControllerAxis.MAX:
                        fmt.println("Axis_Max, TODO")
                        continue
                }
                continue
            }
            case sdl.EventType.CONTROLLERBUTTONDOWN: {
                player := sdl.GameControllerGetPlayerIndex(sdl.GameControllerFromInstanceID(e.cbutton.which))
                controller := input.controllers[player]
                button := e.cbutton.button
                if controller.rebinding {
                    if (controller.button_to_replace != sdl.GameControllerButton.INVALID) {
                        delete_key(controller.controller_map, controller.button_to_replace)
                        controller.button_to_replace = sdl.GameControllerButton.INVALID
                    }
                    controller.controller_map[auto_cast button] = controller.rebind_action
                    controller.rebinding = false
                } else {
                    action := controller.controller_map[auto_cast e.cbutton.button]
                    controller.direct_button_held[auto_cast e.cbutton.button] = true
                    controller.direct_button_pressed[auto_cast e.cbutton.button] = true
                    controller.controller_pressed[action] = true
                    controller.controller_held[action] = true
                }
                continue
            }
            case sdl.EventType.CONTROLLERBUTTONUP: {
                which_controller := sdl.GameControllerGetPlayerIndex(sdl.GameControllerFromInstanceID(e.cbutton.which))
                controller := input.controllers[which_controller]
                action := controller.controller_map[auto_cast e.cbutton.button]
                controller.direct_button_held[e.cbutton.button] = false
                controller.direct_button_pressed[e.cbutton.button] = false
                controller.controller_pressed[action] = false
                controller.controller_held[action] = false
                continue
            }
            case sdl.EventType.MOUSEMOTION: {
                mouse_state := sdl.GetMouseState(&x, &y)
                input.mouse_delta = { x - input.mouse_position.x, y - input.mouse_position.y }
                input.mouse_position = { x,y }
                continue
            }
            case sdl.EventType.MOUSEWHEEL: {
                input.mouse_wheel = { e.wheel.x, e.wheel.y }
                continue
            }
        }
    }
    return false
}
