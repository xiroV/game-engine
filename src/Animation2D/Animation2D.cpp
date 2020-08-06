#include "Animation2D.hpp"
#include <iostream>

AnimationReference::AnimationReference(SDL_Texture *texture, int animation_width, int animation_height, int frames, bool horizontal_texture, bool destroy_on_delete) {
	int w, h;
	if (!SDL_QueryTexture(texture, nullptr, nullptr, &w, &h)) {
		this->texture_width = w;
		this->texture_height = h;
		this->animation_width = animation_width;
		this->animation_height = animation_height;
		this->texture = texture;
		this->frames = frames;
		this->delete_texture_on_destroy = destroy_on_delete;
		this->horizontal_texture = horizontal_texture;
	} else {
		this->texture_width = 0;
		this->texture_height = 0;
		this->animation_width = 0;
		this->animation_height = 0;
		this->texture = nullptr;
		this->frames = -1;
		this->delete_texture_on_destroy = false;
		this->horizontal_texture = true;
		SDL_Log("Texture is invalid");
	}
}

int AnimationReference::get_frames() {
	return this->frames;
}

AnimationReference::~AnimationReference() {
	if (this->delete_texture_on_destroy) {
		SDL_DestroyTexture(this->texture);
	}
}

Animation::Animation(AnimationReference *reference, Uint32 frame_time) {
	this->ref = reference;
	this->frame_time = frame_time;
	this->animation_counter = 0;
	this->wrap_around_value = reference->get_frames() * frame_time;
}

void Animation::get_frame(int *x, int *y, int *w, int *h) {
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

void Animation::update(Uint32 delta) {
	this->animation_counter = (this->animation_counter + delta) % this->wrap_around_value;
}

SDL_Texture* AnimationReference::get_texture() {
	return this->texture;
}

SDL_Texture* Animation::get_texture() {
	return this->ref->get_texture();
}


Animation::~Animation() {}