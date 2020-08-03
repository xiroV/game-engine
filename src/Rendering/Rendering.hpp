#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <map>

#define WHITE SDL_Color{255, 255, 255, SDL_ALPHA_OPAQUE}
#define BLACK SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE}
#define BLUE SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}
#define RED SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE}
#define GREEN SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}

class Rendering {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	TTF_Font *default_font = nullptr;

	public:
		std::map<Uint32, SDL_Texture*> textures;
		Rendering(int width = 1280, int height = 720, bool resizable = false);
		~Rendering();
		const bool has_default_font();
		bool load_default_font(std::string, int);
		const SDL_Renderer* get_renderer();
		const SDL_Window* get_window();
		void render_text(std::string, SDL_Color, int, SDL_Point, TTF_Font* = nullptr);
		inline void render_text(std::string, SDL_Color, int, int, int, TTF_Font* = nullptr);
		int load_and_save_texture(std::string);
		inline void draw_texture(SDL_Texture*, SDL_Rect* = nullptr, SDL_Rect* = nullptr);
		void draw_stored_texture(int, SDL_Rect*, SDL_Rect*);
		SDL_Texture* get_texture(int);
		void set_texture(int, SDL_Texture*);

		void render();
};