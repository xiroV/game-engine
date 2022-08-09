package animationReference

import sdl "vendor:sdl2"

AnimationReference :: struct {
	texture: ^sdl.Texture,
	frames: i32, // Really needed?
	horizontal_texture: bool,
	delete_texture_on_destroy: bool,
	texture_width: i32,
	texture_height: i32,
	animation_width: i32,
	animation_height: i32,
}

initAnimationReference :: proc(texture: ^sdl.Texture, animation_width: i32, animation_height: i32, frames: i32, horizontal_texture: bool, destroy_on_delete: bool) -> (ref: ^AnimationReference, isValid: bool) {
	animationReference := new(AnimationReference)
	w, h: i32
	if sdl.QueryTexture(texture, nil, nil, &w, &h) == 0 {
		animationReference.texture_width = w
		animationReference.texture_height = h
		animationReference.animation_width = animation_width
		animationReference.animation_height = animation_height
		animationReference.texture = texture
		animationReference.frames = frames
		animationReference.delete_texture_on_destroy = destroy_on_delete
		animationReference.horizontal_texture = horizontal_texture
		return animationReference, true
	}
	return nil, false
}

deinitAnimationReference :: proc(animationReference: ^AnimationReference) {
	if animationReference.delete_texture_on_destroy {
		sdl.DestroyTexture(animationReference.texture)
	}
}

get_texture :: proc(animationReference: ^AnimationReference) -> ^sdl.Texture {
    return animationReference.texture
}