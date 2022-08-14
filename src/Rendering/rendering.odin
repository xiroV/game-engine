package rendering

import ani "../animation"
import animRef "../animation_reference"
import sdl "vendor:sdl2"
import ttf "vendor:sdl2/ttf"
import img "vendor:sdl2/image"
import math "core:math"
import fmt "core:fmt"
import strings "core:strings"

WHITE :: sdl.Color{255, 255, 255, sdl.ALPHA_OPAQUE}
BLACK :: sdl.Color{0, 0, 0, sdl.ALPHA_OPAQUE}
BLUE :: sdl.Color{0, 0, 255, sdl.ALPHA_OPAQUE}
RED :: sdl.Color{255, 0, 0, sdl.ALPHA_OPAQUE}
GREEN :: sdl.Color{0, 255, 0, sdl.ALPHA_OPAQUE}

Rendering :: struct {
	window: ^sdl.Window,
	renderer: ^sdl.Renderer,
	default_font: ^ttf.Font,
	textures: map[int]^sdl.Texture,
	texts: map[int]^sdl.Texture,
	scale_x: f64,
	scale_y: f64,
}

initRendering :: proc(width: i32  = 1280, height: i32  = 720, resizable: bool = true) -> ^Rendering {
	rendering := new(Rendering)

	fmt.println(resizable, "not handling resizeable yet")

	flags := sdl.INIT_VIDEO | sdl.INIT_GAMECONTROLLER | sdl.INIT_JOYSTICK | sdl.INIT_AUDIO
	sdl.Init(flags)
	ttf.Init()
	img.Init(img.INIT_PNG)
	sdl.CreateWindowAndRenderer(auto_cast width, auto_cast height, sdl.WINDOW_ALLOW_HIGHDPI, &rendering.window, &rendering.renderer)
	sdl.RenderSetLogicalSize(rendering.renderer, width, height)
	sdl.SetRenderDrawColor(rendering.renderer, 0, 0, 0, sdl.ALPHA_OPAQUE)
	sdl.RenderClear(rendering.renderer)
	return rendering
}

deinitRendering :: proc(rendering: ^Rendering) {
	if has_default_font(rendering) do ttf.CloseFont(rendering.default_font)
	for key, tex in rendering.textures do sdl.DestroyTexture(tex)
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

load_default_font :: proc(rendering: ^Rendering, path: cstring, ptSize: i32) -> bool {
	if rendering.default_font != nil {
		ttf.CloseFont(rendering.default_font)
	}

	loaded_font := ttf.OpenFont(path, ptSize)
	if loaded_font == nil {
		sdl.Log("Failed to load font with path %s", path)
		return false
	}
	rendering.default_font = loaded_font
	return true
}

draw_line :: proc(rendering: ^Rendering, start_x: i64, start_y: i64, to_x: i64, to_y: i64, color: sdl.Color) {
	r, g, b, a: u8
	sdl.GetRenderDrawColor(rendering.renderer, &r, &g, &b, &a)
	sdl.SetRenderDrawColor(rendering.renderer, color.r, color.g, color.b, color.a)
	sdl.RenderDrawLine(rendering.renderer, auto_cast start_x, auto_cast start_y, auto_cast to_x, auto_cast to_y)
	sdl.SetRenderDrawColor(rendering.renderer, r, g, b, a)
}

draw_rect :: proc(rendering: ^Rendering, x: i32, y: i32, width: i32, height: i32, color: sdl.Color, fill: bool) {
	r, g, b, a: u8
	sdl.GetRenderDrawColor(rendering.renderer, &r, &g, &b, &a)
	sdl.SetRenderDrawColor(rendering.renderer, color.r, color.g, color.b, color.a)
	rect := sdl.Rect{x = auto_cast x, y = auto_cast y, w = auto_cast width, h = auto_cast height}
	if fill {
		sdl.RenderFillRect(rendering.renderer, &rect)
	} else {
		sdl.RenderDrawRect(rendering.renderer, &rect)
	}
	sdl.SetRenderDrawColor(rendering.renderer, r, g, b, a)
}

draw_text :: proc(rendering: ^Rendering, text: string, color: sdl.Color, font_size: int, x: int, y: int, font: ^ttf.Font) {
	font_to_use: ^ttf.Font = font != nil ? font : rendering.default_font
	
	str: strings.Builder
    strings.init_builder(&str, context.temp_allocator)
    fmt.sbprintf(&str, text)
    strings.write_byte(&str, 0)

	textSurface := ttf.RenderText_Solid(font_to_use, strings.unsafe_string_to_cstring(strings.to_string(str)), color)
	texture := sdl.CreateTextureFromSurface(rendering.renderer, textSurface)
	position := sdl.Rect{ x = auto_cast x, y = auto_cast y, w = auto_cast (font_size * len(text)), h = auto_cast font_size}
	sdl.RenderCopy(rendering.renderer, texture, nil, &position)
	sdl.DestroyTexture(texture)
	sdl.FreeSurface(textSurface)
}

load_and_save_texture :: proc(using rendering: ^Rendering, path: cstring) -> (key: int) {
	new_texture: ^sdl.Texture = nil
	surface: ^sdl.Surface = img.Load(path)

	if surface == nil do sdl.Log("Unable to load image! sdl.image Error: %s\n", img.GetError())

	key = len(textures)
	textures[key] = sdl.CreateTextureFromSurface(renderer, surface)
	sdl.FreeSurface(surface)
	return
}
draw_texture :: proc(rendering: ^Rendering, texture: ^sdl.Texture,  src: ^sdl.Rect, dest: ^sdl.Rect) {
	sdl.RenderCopy(rendering.renderer, texture, src, dest)
}

draw_animation :: proc{draw_animation_x_y, draw_animation_x_y_w_h}

draw_animation_x_y_w_h :: proc(rendering: ^Rendering, animation: ^ani.Animation, x: i32, y: i32, w: i32, h: i32) {
	a_x, a_y, a_w, a_h := ani.get_frame(animation)
	animation_quad := sdl.Rect{ x = auto_cast a_x, y = auto_cast a_y, w = auto_cast a_w, h = auto_cast a_h }
	location := sdl.Rect{x = auto_cast x, y = auto_cast y, w = auto_cast w, h = auto_cast h}
	draw_texture(rendering, ani.get_texture(animation), &animation_quad, &location)
}

draw_animation_x_y :: proc(rendering: ^Rendering, animation: ^ani.Animation, x: i32, y: i32) {
	a_x, a_y, a_w, a_h := ani.get_frame(animation)
	animation_quad := sdl.Rect{a_x, a_y, a_w, a_h}
	location := sdl.Rect{ x, y, a_w, a_h }
	draw_texture(rendering, ani.get_texture(animation), &animation_quad, &location)
}


draw_stored_texture_quad :: proc(rendering: ^Rendering, key: int, position: ^sdl.Rect, quad_section: ^sdl.Rect) {
	texture := get_texture(rendering, key)
	draw_texture(rendering, texture, quad_section, position)
}

draw_stored_texture_no_quad :: proc(rendering: ^Rendering, key: int, position: ^sdl.Rect) {
	texture := get_texture(rendering, key)
	draw_texture(rendering, texture, nil, position)
}

draw_stored_texture :: proc{draw_stored_texture_no_quad, draw_stored_texture_quad}

store_text_as_texture :: proc(rendering: ^Rendering, text: cstring, font: ^ttf.Font, color: sdl.Color) -> (key: int) {
	font_to_use := font != nil ? font : rendering.default_font
	textSurface := ttf.RenderText_Solid(font_to_use, text, color)
	key = len(rendering.texts)
	rendering.texts[key] = sdl.CreateTextureFromSurface(rendering.renderer, textSurface)
	
	sdl.FreeSurface(textSurface)
	return
}


draw_stored_text :: proc(rendering: ^Rendering, key: int, x: i32, y: i32, font_size: i32, string_length: i32) {
	position := sdl.Rect{ x = auto_cast x, y = auto_cast y, w = auto_cast (font_size * string_length), h = auto_cast font_size}
	sdl.RenderCopy(rendering.renderer, rendering.texts[key], nil, &position)
}

get_stored_text :: proc(rendering: ^Rendering, key: int) -> ^sdl.Texture {
	return rendering.texts[key]
}

draw_texture_rotated :: proc(rendering: ^Rendering, texture: ^sdl.Texture, position: ^sdl.Rect, quad_section: ^sdl.Rect, angle: f64, center: ^sdl.Point, flip: sdl.RendererFlip) {
	sdl.RenderCopyEx(rendering.renderer, texture, quad_section, position, angle, center, flip)
}

draw_stored_texture_rotated :: proc(rendering: ^Rendering, key: int, position: ^sdl.Rect, quad_section: ^sdl.Rect, angle: f64, center: ^sdl.Point, flip: sdl.RendererFlip) {
	texture := get_texture(rendering, key)
	draw_texture_rotated(rendering, texture, position, quad_section, angle, center, flip)
}

get_texture :: proc(rendering: ^Rendering, key: int) -> ^sdl.Texture {
	return rendering.textures[key]
}

set_texture_alpha :: proc{set_texture_alpha_no_key, set_texture_alpha_with_key}

set_texture_alpha_no_key :: proc(texture: ^sdl.Texture, alpha: u8) {
	sdl.SetTextureAlphaMod(texture, alpha)
}

set_texture_alpha_with_key :: proc(rendering: ^Rendering, key: int, alpha: u8) {
	sdl.SetTextureAlphaMod(rendering.textures[key], alpha)
}

draw_animation_rotated :: proc{draw_animation_rotated_x_y, draw_animation_rotated_x_y_w_h}

draw_animation_rotated_x_y :: proc(rendering: ^Rendering, anim: ^ani.Animation, x: i32, y: i32, angle: f64, center: ^sdl.Point, flip: sdl.RendererFlip) {
	a_x, a_y, a_w, a_h := ani.get_frame(anim)
	animation_quad := sdl.Rect{x = a_x, y = a_y, w = a_w, h = a_h}
	location := sdl.Rect{ x = x, y = y, w = a_w, h = a_h }
	draw_texture_rotated(rendering, ani.get_texture(anim), &location, &animation_quad, angle, center, flip)
}

draw_animation_rotated_x_y_w_h :: proc(rendering: ^Rendering, anim: ^ani.Animation, x: i32, y: i32, w: i32, h: i32, angle: f64, center: ^sdl.Point, flip: sdl.RendererFlip) {
	a_x, a_y, a_w, a_h := ani.get_frame(anim)
	animation_quad := sdl.Rect{ a_x, a_y, a_w, a_h }
	location := sdl.Rect{ x, y, w, h }
	draw_texture_rotated(rendering, ani.get_texture(anim), &location, &animation_quad, angle, center, flip)
}

setRGBA :: proc(rendering: ^Rendering, r: u8, g: u8, b: u8, a: u8) {
	sdl.SetRenderDrawColor(rendering.renderer, r, g, b, a)
}

getRGBA :: proc(rendering: ^Rendering, r: ^u8, g: ^u8, b: ^u8, a: ^u8) {
	sdl.GetRenderDrawColor(rendering.renderer, r, g, b, a)
}

show :: proc(rendering: ^Rendering) {
	sdl.RenderPresent(rendering.renderer)
	sdl.RenderClear(rendering.renderer)
}
