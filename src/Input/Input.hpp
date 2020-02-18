#include <map>
#include <SDL2/SDL.h>

typedef map<SDL_Keycode, Action> KeyMap;

bool handle_input(KeyMap &map);
const string keypress_to_name(Action a);

class Input {
    public:
        KeyMap &map;
        Input(KeyMap &map);
        ~Input();
        void bind_key(Action action);
        
};