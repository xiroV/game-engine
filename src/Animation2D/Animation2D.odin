package animation2D

AnimationReference :: struct {
	SDL_Texture* texture;
	int frames; // Really needed?
	bool horizontal_texture;
	bool delete_texture_on_destroy = false;
	int texture_width;
	int texture_height;
	int animation_width;
	int animation_height;

	init :: proc(SDL_Texture *texture, int animation_width, int animation_height, int frames, bool horizontal_texture, bool destroy_on_delete) -> (ref: ^AnimationReference, isValid: bool) {
		animationReference := new(AnimationReference)
		int w, h;
		if (!SDL_QueryTexture(texture, nullptr, nullptr, &w, &h)) {
			animationReference.texture_width = w;
			animationReference.texture_height = h;
			animationReference.animation_width = animation_width;
			animationReference.animation_height = animation_height;
			animationReference.texture = texture;
			animationReference.frames = frames;
			animationReference.delete_texture_on_destroy = destroy_on_delete;
			animationReference.horizontal_texture = horizontal_texture;
			return animationReference, true
		}
		return nil, false
	}
	
	deinit :: proc(animationReference: ^AnimationReference) {
		if animationReference.delete_texture_on_destroy {
			SDL_DestroyTexture(animationReference.texture)
		}
	};
};

get_texture :: proc(animationReference: ^AnimationReference) -> ^SDL_Texture {
    return animationReference.texture;
}

Animation :: struct {
	ref: ^AnimationReference
	animation_counter: u32
	frame_time: u32
	wrap_around_value: u32
}

get_frame :: proc(animation: ^Animation, x: int, y: int, w: int, h: int) -> (x: int, y: int, w: int, h: int) {
    int frame = this->animation_counter / frame_time;

    w := animation.ref.animation_width;
    h := animation.ref.animation_height;
    y := 0
    x := 0

    if (animation.ref.horizontal_texture) {
        x = frame * animation.ref.animation_width;
        y = 0;
    } else {
        x = 0;
        y = frame * animation.ref.animation_height;
    }
    return x, y, w, h
}
	
update :: proc(animation: ^Animation, delta: u32) {
    animation.animation_counter = (animation.animation_counter + delta) % animation.wrap_around_value;
}
	
get_texture :: proc(animation: ^Animation) -> ^SDL_Texture {
    return animation.ref.get_texture()
}

init :: proc(reference: ^AnimationReference, frame_time: u32) -> Animation {
    animation := new(Animation)
    animation.ref = reference;
    animation.frame_time = frame_time;
    animation.animation_counter = 0;
    animation.wrap_around_value = reference->get_frames() * frame_time;
    return animation
}

deinit :: proc() {
    // TODO
}


get_frame(animation: ^Animation, int *x, int *y, int *w, int *h) {
	int frame = this->animation_counter / frame_time;

	*w = this->ref->animation_width;
	*h = this->ref->animation_height;

	if (this->ref->horizontal_texture) {
		*x = frame * this->ref->animation_width;
		*y = 0;
	} else {
		*x = 0;
		*y = frame * this->ref->animation_height;
	}
}

update :: proc(animation: ^Animation, delta: u32) {
	this->animation_counter = (this->animation_counter + delta) % this->wrap_around_value;
}

get_texture :: proc() -> ^Texture {
    return ref.texture
}
