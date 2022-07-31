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
}
