package animation

import sdl "vendor:sdl2"
import animRef "../animation_reference"

Animation :: struct {
	ref: ^animRef.AnimationReference,
	animation_counter: u32,
	frame_time: u32,
	wrap_around_value: u32,
}

initAnimation :: proc(reference: ^animRef.AnimationReference, frame_time: u32) -> ^Animation {
	animation := new(Animation)
	animation.ref = reference
	animation.frame_time = frame_time
	animation.animation_counter = 0
	animation.wrap_around_value = reference.frames * frame_time
	return animation
}

deinitAnimation :: proc(ref: ^animRef.AnimationReference) {
	// TODO
}

get_frame :: proc(animation: ^Animation) -> (i32, i32, i32, i32) {
    frame := animation.animation_counter / animation.frame_time

    w: i32 = animation.ref.animation_width
    h: i32 = animation.ref.animation_height
    y: i32 = 0
    x: i32 = 0

    if (animation.ref.horizontal_texture) {
        x = frame * animation.ref.animation_width
        y = 0
    } else {
        x = 0
        y = frame * animation.ref.animation_height
    }
    return x, y, w, h
}
	
update :: proc(animation: ^Animation, delta: u32) {
    animation.animation_counter = (animation.animation_counter + delta) % animation.wrap_around_value
}

get_texture :: proc(animation: ^Animation) -> ^sdl.Texture {
    return animation.ref.texture
}
