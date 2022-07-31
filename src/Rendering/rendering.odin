package rendering

import anim "animation2D"
import sdl "vendor:sdl"
import ttf "vendor:sdl/ttf"

WHITE :: sdl.Color{255, 255, 255, sdl.ALPHA_OPAQUE}
BLACK :: sdl.Color{0, 0, 0, sdl.ALPHA_OPAQUE}
BLUE :: sdl.Color{0, 0, 255, sdl.ALPHA_OPAQUE}
RED :: sdl.Color{255, 0, 0, sdl.ALPHA_OPAQUE}
GREEN :: sdl.Color{0, 255, 0, sdl.ALPHA_OPAQUE}

Rendering :: struct {
	window: ^sdl.Window = nil;
	renderer: ^sdl.Renderer = nil;
	default_font: ^TTF_Font = nil;
	textures: [dynamic]^sdl.Texture
	texts: [dynamic]^sdl.Texture
	scale_x: f64 = 1.0
	scale_y: f64 = 1.0
}

init :: proc(width: int  = 1280, height: int  = 720, resizable: bool = true) -> {
	rendering := new(Rendering)
	sdl.Init(sdl.INIT_VIDEO | sdl.INIT_GAMECONTROLLER | sdl.INIT_JOYSTICK | sdl.INIT_AUDIO | (resizable ? sdl.WINDOW_RESIZABLE : 0));
	ttf.Init();
	IMG_Init(IMG_INIT_PNG);
	sdl.Window *window;
	sdl.Renderer *renderer;
	sdl.CreateWindowAndRenderer(width, height, sdl.WINDOW_ALLOW_HIGHDPI, &window, &renderer);
	sdl.RenderSetLogicalSize(renderer, width, height);
	sdl.SetRenderDrawColor(renderer, 0, 0, 0, sdl.ALPHA_OPAQUE);
	sdl.RenderClear(renderer)
	rendering.window = window
	rendering.renderer = renderer
	return rendering
}

deinit :: proc(rendering: ^Rendering) {
	if rendering.has_default_font() do ttf.CloseFont(rendering.default_font)
	for t in rendering.textures do sdl.DestroyTexture(t.second);
	ttf.Quit()
	img.Quit()
	sdl.DestroyRenderer(rendering.renderer)
	sdl.DestroyWindow(rendering.window)
	sdl.Quit()
	free(rendering)
}

has_default_font :: proc(rendering: ^Rendering) -> bool {
	return rendering.default_font != nil
}

load_default_font :: proc(rendering: ^Rendering, path: string, ptSize: int) -> bool {
	if rendering.default_font != nil {
		ttf.CloseFont(this->default_font);
	}

	loaded_font := TTF_OpenFont(path.c_str(), ptSize)
	if (loaded_font == nullptr) {
		sdl.Log("Failed to load font with path %s", path.c_str());
		return false;
	}
	rendering.default_font = loaded_font;
	return true;
}

draw_line :: proc(rendering: ^Rendering, int start_x, int start_y, int to_x, int to_y, sdl.Color color) {
	r, g, b, a: u8
	sdl.GetRenderDrawColor(rendering.renderer, &r, &g, &b, &a);
	sdl.SetRenderDrawColor(rendering.renderer, color.r, color.g, color.b, color.a);
	sdl.RenderDrawLine(rendering.renderer, start_x, start_y, to_x, to_y);
	sdl.SetRenderDrawColor(rendering.renderer, r, g, b, a)
}

draw_rect :: proc(x: int, y: int, int width, int height, sdl.Color color, bool fill) {
	Uint8 r, g, b, a;
	sdl.GetRenderDrawColor(this->renderer, &r, &g, &b, &a);
	sdl.SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
	sdl.Rect rect = sdl.Rect{x, y, width, height};
	if fill {
		sdl.RenderFillRect(this->renderer, &rect);
	} else {
		sdl.RenderDrawRect(this->renderer, &rect);
	}
	sdl.SetRenderDrawColor(this->renderer, r, g, b, a);
}

draw_text :: proc(std::string text, sdl.Color color, int font_size, int x, int y, TTF_Font *font) {
	TTF_Font *font_to_use = font != nullptr ? font : this->default_font;
	sdl.Surface* textSurface = TTF_RenderText_Solid(font_to_use, text.c_str(), color);
	texture := sdl.CreateTextureFromSurface(this->renderer, textSurface);
	position := sdl.Rect{ x, y, (int)floor(font_size * text.length()), font_size };
	sdl.RenderCopy(this->renderer, texture, NULL, &position);
	sdl.DestroyTexture(texture);
	sdl.FreeSurface(textSurface);
}	

load_and_save_texture :: proc(rendering: ^Rendering, path: string) -> (id: uint) {
	new_texture: ^sdl.Texture = nil
	surface: ^sdl.Surface = IMG_Load(path.c_str())

	if surface == nil {
		sdl.Log("Unable to load image! sdl.image Error: %s\n", IMG_GetError());
	}

	i := 0;
	for rendering.textures[i] != nil; i += 1 {}
	set_texture(rendering, i, sdl.CreateTextureFromSurface(rendering.renderer, surface));
	sdl.FreeSurface(surface);
	return i;
}
draw_texture :: proc(sdl.Texture*, sdl.Rect*, sdl.Rect*) {
	sdl.RenderCopy(this->renderer, texture, src, dest);
}

draw_animation :: proc(Animation *anim, int x, int y, int w, int h) {
	int a_x, a_y, a_w, a_h;
	anim->get_frame(&a_x, &a_y, &a_w, &a_h);
	sdl.Rect animation_quad = { a_x, a_y, a_w, a_h };
	sdl.Rect location = {x, y, w, h };
	this->draw_texture(anim->get_texture(), &animation_quad, &location);
}

draw_animation :: proc(Animation *anim, int x, int y) {
	int a_x, a_y, a_w, a_h;
	anim.get_frame(&a_x, &a_y, &a_w, &a_h);
	animation_quad := sdl.Rect{a_x, a_y, a_w, a_h};
	sdl.Rect location = { x, y, a_w, a_h };
	this->draw_texture(anim->get_texture(), &animation_quad, &location);
}

void draw_stored_texture(int key, sdl.Rect *position, sdl.Rect *quad_section) {
	auto *texture = this->get_texture(key);
	this->draw_texture(texture, quad_section, position);
}

void draw_stored_texture(int key, sdl.Rect *position) {
	auto *texture = this->get_texture(key);
	this->draw_texture(texture, nullptr, position);
}

store_text_as_texture(std::string text, TTF_Font *font, sdl.Color color) -> uint {
	TTF_Font* font_to_use = font != nullptr ? font : this->default_font;
	sdl.Surface* textSurface = TTF_RenderText_Solid(font_to_use, text.c_str(), color);
	sdl.Texture* texture = sdl.CreateTextureFromSurface(this->renderer, textSurface);
	int i = 0;
	for this->texts[i] != nil { i += 1 }
	this->texts[i] = texture;
	return i;
}


draw_stored_text :: proc(int key, int x, int y, int font_size, int string_length) {
	sdl.Rect position = { x, y, font_size * string_length, font_size };
	sdl.RenderCopy(this->renderer, this->texts[key], NULL, &position);
}

get_stored_text :: proc(int key) -> ^sdl.Texture {
	return this->texts[key];
}

set_text :: proc(key: int, texture: ^sdl.Texture) {
	this->texts[key] = texture;
}

draw_texture_rotated :: proc(sdl.Texture* texture, sdl.Rect* position, sdl.Rect* quad_section, double angle, sdl.Point* center, sdl.RendererFlip flip) {
	sdl.RenderCopyEx(this->renderer, texture, quad_section, position, angle, center, flip);
}

draw_stored_texture_rotated :: proc (int key, sdl.Rect* position, sdl.Rect* quad_section, double angle, sdl.Point* center, sdl.RendererFlip flip) {
	auto* texture = this->get_texture(key);
	this->draw_texture_rotated(texture, position, quad_section, angle, center, flip);
}

sdl.Texture* Rendering::get_texture(int key) {
	return this->textures[key];
}

void Rendering::set_texture(int key, sdl.Texture *texture) {
	this->textures[key] = texture;
}

set_texture_alpha :: proc(texture: ^sdl.Texture, alpha: u8) {
	sdl.SetTextureAlphaMod(texture, alpha);
}

set_texture_alpha(key: uint, alpha: u8) {
	auto *texture = this->textures[key];
	sdl.SetTextureAlphaMod(texture, alpha);
}

draw_animation_rotated :: proc(Animation *anim, int x, int y, double angle, sdl.Point* center, sdl.RendererFlip flip) {
	int a_x, a_y, a_w, a_h;
	anim->get_frame(&a_x, &a_y, &a_w, &a_h);
	sdl.Rect animation_quad = { a_x, a_y, a_w, a_h };
	sdl.Rect location = { x, y, a_w, a_h };
	this->draw_texture_rotated(anim->get_texture(), &location, &animation_quad, angle, center, flip);
}

draw_animation_rotated :: proc(Animation *anim, int x, int y, int w, int h, double angle, sdl.Point *center, sdl.RendererFlip flip) {
	int a_x, a_y, a_w, a_h;
	anim->get_frame(&a_x, &a_y, &a_w, &a_h);
	sdl.Rect animation_quad = { a_x, a_y, a_w, a_h };
	sdl.Rect location = { x, y, w, h };
	this->draw_texture_rotated(anim->get_texture(), &location, &animation_quad, angle, center, flip);
}

void Rendering::setRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	sdl.SetRenderDrawColor(this->renderer, r, g, b, a);
}

void Rendering::getRGBA(Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
	sdl.GetRenderDrawColor(this->renderer, r, g, b, a);
}

show :: proc(rendering: ^Rendering) {
	sdl.RenderPresent(rendering.renderer);
	sdl.RenderClear(rendering.renderer);
}