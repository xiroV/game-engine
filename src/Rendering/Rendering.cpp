#include "Rendering.hpp"

Rendering::Rendering(int width, int height, bool resizable) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | (resizable ? SDL_WINDOW_RESIZABLE : 0));
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
	SDL_RenderSetLogicalSize(renderer, width, height);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	this->window = window;
	this->renderer = renderer;
}

void Rendering::draw_line(int start_x, int start_y, int to_x, int to_y, SDL_Color color) {
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(this->renderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(this->renderer, start_x, start_y, to_x, to_y);
	SDL_SetRenderDrawColor(this->renderer, r, g, b, a);

}

void Rendering::draw_rect(int x, int y, int width, int height, SDL_Color color, bool fill) {
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(this->renderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
	SDL_Rect rect = SDL_Rect{x, y, width, height};
	if (fill) {
		SDL_RenderFillRect(this->renderer, &rect);
	} else {
		SDL_RenderDrawRect(this->renderer, &rect);
	}
	SDL_SetRenderDrawColor(this->renderer, r, g, b, a);
}

void Rendering::show() {
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
}

Rendering::~Rendering() {
	if (this->has_default_font()) TTF_CloseFont(this->default_font);
	for (auto t : this->textures) SDL_DestroyTexture(t.second);
	this->textures;
	TTF_Quit();
	IMG_Quit();
	// SDL_DestroyRenderer();
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool Rendering::load_default_font(std::string path, int ptsize) {
	if (this->default_font != nullptr) {
		TTF_CloseFont(this->default_font);
	}

	auto *loaded_font = TTF_OpenFont(path.c_str(), 32);
	if (loaded_font == nullptr) {
		SDL_Log("Failed to load font with path %s", path.c_str());
		return false;
	}
	this->default_font = loaded_font;
	return true;
}

const SDL_Renderer* Rendering::get_renderer() {
	return this->renderer;
}

const SDL_Window* Rendering::get_window() {
	return this->window;
}

bool Rendering::has_default_font() {
	return this->default_font == nullptr;
}

void Rendering::draw_text(std::string text, SDL_Color color, int font_size, int x, int y, TTF_Font *font) {
	TTF_Font *font_to_use = font != nullptr ? font : this->default_font;
	auto f1 = font;
	auto f2 = this->default_font;
	SDL_Surface* textSurface = TTF_RenderText_Solid(font_to_use, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
	SDL_Rect position = { x, y, (int)floor(font_size * text.length()), font_size };
	SDL_RenderCopy(this->renderer, texture, NULL, &position);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(textSurface);
}

inline void Rendering::draw_texture(SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dest) {
	SDL_RenderCopy(this->renderer, texture, src, dest);
}

void Rendering::draw_stored_texture(int key, SDL_Rect *texture_section, SDL_Rect* position) {
	auto* texture = this->get_texture(key);
	this->draw_texture(texture, texture_section, position);
}


SDL_Texture* Rendering::get_texture(int key) {
	return this->textures[key];
}

void Rendering::set_texture(int key, SDL_Texture *texture) {
	this->textures[key] = texture;
}

int Rendering::load_and_save_texture(std::string path) {
	SDL_Texture* new_texture = nullptr;
	SDL_Surface* surface = IMG_Load(path.c_str());
	
	if (surface == nullptr) {
		SDL_Log("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
	}

	int i = -1;
	while (this->textures[++i] != nullptr);
	this->set_texture(i, SDL_CreateTextureFromSurface(this->renderer, surface));
	SDL_FreeSurface(surface);
	return i;
}
