#include <map>
#include <SDL2/SDL.h>

enum InputState {
    Listening,
    Rebinding
};

enum UserAction {
    MoveLeft = 1,
    MoveRight,
    Jump,
    Attack
};

typedef std::map<SDL_Keycode, UserAction> KeyMap;
typedef std::map<UserAction, bool> KeyPressMap;

const std::string keypress_to_name(UserAction a);

class Input {
    private:
        bool rebind_finished = false;
        UserAction rebind_action; // I guess this should be public?
    public:
        SDL_Point mouse_position {0,0};
        SDL_Point mouse_delta {0,0};
        void handle_input();
        InputState state = Listening;
        void set_action_to_rebind(UserAction action);
        KeyMap &map;
        KeyPressMap &keys_held_down;
        KeyPressMap &keys_pressed_once;
        bool is_down(Action action);
        bool is_pressed(Action action);
        Input(KeyMap &map);
        ~Input();
        void bind_key(SDL_Keycode key);
};