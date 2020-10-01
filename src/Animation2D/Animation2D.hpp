#include <SDL2/SDL.h>

class AnimationReference {
	private:
		SDL_Texture* texture;
		int frames; // Really needed?
	public:
		bool horizontal_texture;
		bool delete_texture_on_destroy = false;
		int texture_width;
		int texture_height;
		int animation_width;
		int animation_height;
		int get_frames();
		SDL_Texture *get_texture();
		AnimationReference(SDL_Texture *texture, int animation_width, int animation_height, int frames, bool horizontal_texture, bool destroy_on_delete);
		~AnimationReference();
};

class Animation {
	private:
		AnimationReference* ref;
		Uint32 animation_counter;
		Uint32 frame_time;
		Uint32 wrap_around_value;
	public:
		void get_frame(int* x, int* y, int* w, int* h);
		void update(Uint32 delta);
		SDL_Texture *get_texture();
		Animation(AnimationReference* reference, Uint32 frame_time);
		~Animation();
};
