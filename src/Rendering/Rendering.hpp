#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <map>
#include <cmath>
#include "../Animation2D/Animation2D.hpp"

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
		std::map<Uint32, SDL_Texture*> texts;

		/// <summary>
		///	Constructor for Rendering class. Defaults to 1280x720 in resolution with resizing disallowed.
		/// </summary>
		/// <param name="width">Window width of window to render content</param>
		/// <param name="height">Window height of window to render content</param>
		/// <param name="resizable">Whether or not the window can be resized</param>
		/// <returns></returns>
		Rendering(int width = 1280, int height = 720, bool resizable = false);
		~Rendering();
		double scale_x = 1;
		double scale_y = 1;
		bool has_default_font();
		bool load_default_font(std::string, int);
		const SDL_Renderer* get_renderer();
		const SDL_Window* get_window();
		void draw_line(int, int, int, int, SDL_Color);
		void draw_rect(int x, int y, int width, int height, SDL_Color color, bool fill);
		void draw_text(std::string, SDL_Color, int, int, int, TTF_Font* = nullptr);
		int load_and_save_texture(std::string);
		inline void draw_texture(SDL_Texture*, SDL_Rect*, SDL_Rect*);
		void draw_animation(Animation* anim, int x, int y, int w, int h);
		void draw_animation(Animation* anim, int x, int y);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="key"></param>
		/// <param name="position"></param>
		/// <param name="quad_section"></param>
		void draw_stored_texture(int key, SDL_Rect *position, SDL_Rect *quad_section);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="key"></param>
		/// <param name="position"></param>
		void draw_stored_texture(int key, SDL_Rect *position);
		int store_text_as_texture(std::string text, TTF_Font *font, SDL_Color color);
		void draw_stored_text(int key, int x, int y, int font_size, int string_length);
		SDL_Texture* get_stored_text(int key);
		void set_text(int key, SDL_Texture *texture);

		SDL_Texture* get_texture(int key);
		inline void draw_texture_rotated(SDL_Texture* texture, SDL_Rect* position, SDL_Rect* quad_section, double angle, SDL_Point* center, SDL_RendererFlip flip);
		void draw_stored_texture_rotated(int key, SDL_Rect* position, SDL_Rect* quad_section, double angle, SDL_Point* center, SDL_RendererFlip flip);
		void set_texture(int, SDL_Texture*);
		void set_texture_alpha(int key, Uint8 alpha);
		void set_texture_alpha(SDL_Texture* texture, Uint8 alpha);

		void draw_animation_rotated(Animation *anim, int x, int y, double angle, SDL_Point *center, SDL_RendererFlip flip);
		void draw_animation_rotated(Animation* anim, int x, int y, int w, int h, double angle, SDL_Point* center, SDL_RendererFlip flip);

		void setRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void getRGBA(Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);

		void show();
};