#include <map>
#include <SDL2/SDL.h>

enum InputState {
    Listening,
    Rebinding
};

enum Action {
    MoveLeft = 1,
    MoveRight,
    Jump,
    Attack
};

typedef std::map<SDL_Keycode, Action> KeyMap;


bool handle_input(KeyMap &map);
const std::string keypress_to_name(Action a);

class Input {
    public:
        InputState state;
        KeyMap &map;
        Input(KeyMap &map);
        ~Input();
        void bind_key(Action action);
        void poll_for_keys();
};