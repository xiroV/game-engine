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
		/// <summary>
		///	Constructor for Rendering class. Defaults to 1280x720 in resolution with resizing disallowed.
		/// </summary>
		/// <param name="width">Window width of window to render content</param>
		/// <param name="height">Window height of window to render content</param>
		/// <param name="resizable">Whether or not the window can be resized</param>
		/// <returns></returns>
		Rendering(int width = 1280, int height = 720, bool resizable = false);
		double scale_x = 1;
		double scale_y = 1;
		~Rendering();
		bool has_default_font();
		bool load_default_font(std::string, int);
		const SDL_Renderer* get_renderer();
		const SDL_Window* get_window();
		void draw_line(int, int, int, int, SDL_Color);
		void draw_rect(int x, int y, int width, int height, SDL_Color color, bool fill);
		void draw_text(std::string, SDL_Color, int, int, int, TTF_Font* = nullptr);
		int load_and_save_texture(std::string);
		inline void draw_texture(SDL_Texture*, SDL_Rect*, SDL_Rect*);
		void draw_stored_texture(int, SDL_Rect*, SDL_Rect*);
		SDL_Texture* get_texture(int);
		void set_texture(int, SDL_Texture*);

		void show();
};