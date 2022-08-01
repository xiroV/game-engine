package rendering

import anim "animation2D"
import sdl "vendor:sdl"
import ttf "vendor:sdl/ttf"
import img "vendor:sdl/img"

WHITE :: sdl.Color{255, 255, 255, sdl.ALPHA_OPAQUE}
BLACK :: sdl.Color{0, 0, 0, sdl.ALPHA_OPAQUE}
BLUE :: sdl.Color{0, 0, 255, sdl.ALPHA_OPAQUE}
RED :: sdl.Color{255, 0, 0, sdl.ALPHA_OPAQUE}
GREEN :: sdl.Color{0, 255, 0, sdl.ALPHA_OPAQUE}

Rendering :: struct {
	window: ^sdl.Window,
	renderer: ^sdl.Renderer,
	default_font: ^TTF_Font,
	textures: [dynamic]^sdl.Texture,
	texts: [dynamic]^sdl.Texture,
	scale_x: f64,
	scale_y: f64,
}

init :: proc(width: int  = 1280, height: int  = 720, resizable: bool = true) -> ^Rendering {
	rendering := new(Rendering)
	sdl.Init(sdl.INIT_VIDEO | sdl.INIT_GAMECONTROLLER | sdl.INIT_JOYSTICK | sdl.INIT_AUDIO | (resizable ? sdl.WINDOW_RESIZABLE : 0));
	ttf.Init();
	img.Init(img.INIT_PNG);
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
	if loaded_font == nil {
		sdl.Log("Failed to load font with path %s", path.c_str());
		return false;
	}
	rendering.default_font = loaded_font;
	return true;
}

draw_line :: proc(rendering: ^Rendering, start_x: i64, start_y: i64, to_x: i64, to_y: i64, color: sdl.Color) {
	r, g, b, a: u8
	sdl.GetRenderDrawColor(rendering.renderer, &r, &g, &b, &a);
	sdl.SetRenderDrawColor(rendering.renderer, color.r, color.g, color.b, color.a);
	sdl.RenderDrawLine(rendering.renderer, start_x, start_y, to_x, to_y);
	sdl.SetRenderDrawColor(rendering.renderer, r, g, b, a)
}

draw_rect :: proc(rendering: ^Rendering, x: int, y: int, width: int, height: int, color: sdl.Color, fill: bool) {
	r, g, b, a: u8;
	sdl.GetRenderDrawColor(rendering.renderer, &r, &g, &b, &a);
	sdl.SetRenderDrawColor(rendering.renderer, color.r, color.g, color.b, color.a)
	rect := sdl.Rect{x = x, y = y, w = width, h = height}
	if fill {
		sdl.RenderFillRect(rendering.renderer, &rect);
	} else {
		sdl.RenderDrawRect(rendering.renderer, &rect);
	}
	sdl.SetRenderDrawColor(rendering.renderer, r, g, b, a);
}

draw_text :: proc(rendering: ^Rendering, text: string, color: sdl.Color, font_size: int, x: int, y: int, font: ^ttf.Font) {
	font_to_use: ^ttf.Font = font != nil ? font : rendering.default_font;
	textSurface = ttf.RenderText_Solid(font_to_use, text.c_str(), color);
	texture := sdl.CreateTextureFromSurface(rendering.renderer, textSurface);
	position := sdl.Rect{ x = x, y = y, w = auto_cast floor(font_size * text.length()), h = font_size};
	sdl.RenderCopy(rendering.renderer, texture, NULL, &position);
	sdl.DestroyTexture(texture);
	sdl.FreeSurface(textSurface);
}	

load_and_save_texture :: proc(rendering: ^Rendering, path: string) -> (id: uint) {
	new_texture: ^sdl.Texture = nil
	surface: ^sdl.Surface = IMG_Load(path.c_str())

	if surface == nil {
		sdl.Log("Unable to load image! sdl.image Error: %s\n", IMG_GetError());
	}

    append(rendering.textures, sdl.CreateTextureFromSurface(rendering.renderer, surface)) 
	sdl.FreeSurface(surface);
	return rendering.textures.count - 1;
}
draw_texture :: proc(rendering: ^Rendering, texture: ^sdl.Texture,  src: ^sdl.Rect, dest: ^sdl.Rect) {
	sdl.RenderCopy(this->renderer, texture, src, dest);
}

draw_animation :: proc(anim: ^Animation, x: int, y: int, w: int, h: int) {
	a_x, a_y, a_w, a_h := get_frame(anim)
	animation_quad := sdl.Rect{ x = a_x, y = a_y, w = a_w, h = a_h }
	location := sdl.Rect{x = x, y = y, w = w, h = h}
	draw_texture(get_texture(anim), &animation_quad, &location)
}

draw_animation :: proc(anim: ^Animation, x: int, y: int) {
	a_x, a_y, a_w, a_h := get_frame(anim);
	animation_quad := sdl.Rect{a_x, a_y, a_w, a_h};
	location = sdl.Rect{ x, y, a_w, a_h };
	draw_texture(anim, get_texture(anim), &animation_quad, &location);
}

draw_stored_texture :: proc(rendering: ^Rendering, key: int, position: ^sdl.Rect, quad_section: ^sdl.Rect) {
	texture := get_texture(rendering, key);
	draw_texture(rendering, texture, quad_section, position);
}

draw_stored_texture :: proc(rendering: ^Rendering, key: int, position: ^sdl.Rect) {
	texture := get_texture(rendering, key);
	draw_texture(rendering, texture, nullptr, position);
}

store_text_as_texture :: proc(rendering: ^Rendering, text: string, font: ^ttf.Font, color: sdl.Color) -> uint {
	font_to_use := font != nil ? font : rendering.default_font
	textSurface := TTF_RenderText_Solid(font_to_use, text.c_str(), color)
	texture := sdl.CreateTextureFromSurface(this->renderer, textSurface)
	append(rendering.texts, texture)
	return rendering.texts.count
}


draw_stored_text :: proc(rendering: ^Rendering, key: int, x: int, y: int, font_size: int, string_length: int) {
	position := sdl.Rect{ x = x, y = y, w = font_size * string_length, h = font_size}
	sdl.RenderCopy(rendering.renderer, rendering.texts[key], nil, &position)
}

get_stored_text :: proc(rendering: ^Rendering, key: int) -> ^sdl.Texture {
	return rendering.texts[key];
}

draw_texture_rotated :: proc(texture: ^sdl.Texture, position: ^sdl.Rect, quad_section: ^sdl.Rect, angle: f64, center: ^sdl.Point, flip: sdl.RendererFlip) {
	sdl.RenderCopyEx(this->renderer, texture, quad_section, position, angle, center, flip);
}

draw_stored_texture_rotated :: proc(key: u64, position: ^sdl.Rect, quad_section: ^sdl.Rect, angle: f64, center: ^sdl.Point, flip: sdl.RendererFlip) {
	auto* texture = this->get_texture(key);
	this->draw_texture_rotated(texture, position, quad_section, angle, center, flip);
}

get_texture :: proc(rendering: ^Rendering, key: int) -> ^sdl.Texture {
	return rendering.textures[key];
}

set_texture :: proc(rendering: ^Rendering, int key, sdl.Texture *texture) {
	rendering.textures[key] = texture;
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