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
	SDL_Surface* textSurface = TTF_RenderText_Solid(font_to_use, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
	SDL_Rect position = { x, y, (int)floor(font_size * text.length()), font_size };
	SDL_RenderCopy(this->renderer, texture, NULL, &position);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(textSurface);
}

int Rendering::store_text_as_texture(std::string text, TTF_Font *font, SDL_Color color) {
	TTF_Font* font_to_use = font != nullptr ? font : this->default_font;
	SDL_Surface* textSurface = TTF_RenderText_Solid(font_to_use, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
	int i = -1;
	while (this->texts[++i] != nullptr);
	this->texts[i] = texture;
	return i;
}

void Rendering::draw_stored_text(int key, int x, int y, int font_size, int string_length) {
	SDL_Rect position = { x, y, font_size * string_length, font_size };
	SDL_RenderCopy(this->renderer, this->texts[key], NULL, &position);
}

SDL_Texture* Rendering::get_stored_text(int key) {
	return this->texts[key];
}

void Rendering::set_text(int key, SDL_Texture* texture) {
	this->texts[key] = texture;
}


inline void Rendering::draw_texture(SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dest) {
	SDL_RenderCopy(this->renderer, texture, src, dest);
}

inline void Rendering::draw_texture_rotated(SDL_Texture *texture, SDL_Rect *position, SDL_Rect *quad_section, double angle, SDL_Point *center, SDL_RendererFlip flip) {
	SDL_RenderCopyEx(this->renderer, texture, quad_section, position, angle, center, flip);
}

void Rendering::draw_stored_texture(int key, SDL_Rect *position, SDL_Rect *quad_section) {
	auto *texture = this->get_texture(key);
	this->draw_texture(texture, quad_section, position);
}

void Rendering::draw_stored_texture(int key, SDL_Rect *position) {
	auto *texture = this->get_texture(key);
	this->draw_texture(texture, nullptr, position);
}

void Rendering::draw_stored_texture_rotated(int key, SDL_Rect* position, SDL_Rect* quad_section, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	auto* texture = this->get_texture(key);
	this->draw_texture_rotated(texture, position, quad_section, angle, center, flip);
}

SDL_Texture* Rendering::get_texture(int key) {
	return this->textures[key];
}

void Rendering::set_texture(int key, SDL_Texture *texture) {
	this->textures[key] = texture;
}

void Rendering::setRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(this->renderer, r, g, b, a);
}

void Rendering::getRGBA(Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
	SDL_GetRenderDrawColor(this->renderer, r, g, b, a);
}

void Rendering::set_texture_alpha(SDL_Texture *texture, Uint8 alpha) {
	SDL_SetTextureAlphaMod(texture, alpha);
}

void Rendering::set_texture_alpha(int key, Uint8 alpha) {
	auto *texture = this->textures[key];
	SDL_SetTextureAlphaMod(texture, alpha);
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


void Rendering::draw_animation(Animation *anim, int x, int y, int w, int h) {
	int a_x, a_y, a_w, a_h;
	anim->get_frame(&a_x, &a_y, &a_w, &a_h);
	SDL_Rect animation_quad = { a_x, a_y, a_w, a_h };
	SDL_Rect location = {x, y, w, h };
	this->draw_texture(anim->get_texture(), &animation_quad, &location);
}

void Rendering::draw_animation(Animation *anim, int x, int y) {
	int a_x, a_y, a_w, a_h;
	anim->get_frame(&a_x, &a_y, &a_w, &a_h);
	SDL_Rect animation_quad = {a_x, a_y, a_w, a_h};
	SDL_Rect location = { x, y, a_w, a_h };
	this->draw_texture(anim->get_texture(), &animation_quad, &location);
}

void Rendering::draw_animation_rotated(Animation *anim, int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	int a_x, a_y, a_w, a_h;
	anim->get_frame(&a_x, &a_y, &a_w, &a_h);
	SDL_Rect animation_quad = { a_x, a_y, a_w, a_h };
	SDL_Rect location = { x, y, a_w, a_h };
	this->draw_texture_rotated(anim->get_texture(), &location, &animation_quad, angle, center, flip);
}

void Rendering::draw_animation_rotated(Animation *anim, int x, int y, int w, int h, double angle, SDL_Point *center, SDL_RendererFlip flip) {
	int a_x, a_y, a_w, a_h;
	anim->get_frame(&a_x, &a_y, &a_w, &a_h);
	SDL_Rect animation_quad = { a_x, a_y, a_w, a_h };
	SDL_Rect location = { x, y, w, h };
	this->draw_texture_rotated(anim->get_texture(), &location, &animation_quad, angle, center, flip);
}
