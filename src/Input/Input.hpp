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

const std::string keypress_to_name(UserAction a);

class Input {
    private:
        bool rebind_finished = false;
        UserAction rebind_action; // I guess this should be public?
    public:
        void handle_input();
        InputState state = Listening;
        void set_action_to_rebind(UserAction action);
        KeyMap &map;
        void input();
        Input(KeyMap &map);
        ~Input();
        void bind_key(SDL_Keycode key);
};