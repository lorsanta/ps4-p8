#pragma once

#include <SDL2/SDL.h>
#include <vector>

#ifdef __PS4__
#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>

#define FRAME_WIDTH     1920
#define FRAME_HEIGHT    1080
#else
// SDL messes up with the linker somehow on windows?
#undef main
#define FRAME_WIDTH     1440
#define FRAME_HEIGHT    810
#endif

#define P8_WIDTH 128
#define P8_HEIGHT 128

static SDL_Color PALETTE[] = {
	SDL_Color { 0x00, 0x00, 0x00, 0xff },
	SDL_Color { 0x1d, 0x2b, 0x53, 0xff },
	SDL_Color { 0x7e, 0x25, 0x53, 0xff },
	SDL_Color { 0x00, 0x87, 0x51, 0xff },
	SDL_Color { 0xab, 0x52, 0x36, 0xff },
	SDL_Color { 0x5f, 0x57, 0x4f, 0xff },
	SDL_Color { 0xc2, 0xc3, 0xc7, 0xff },
	SDL_Color { 0xff, 0xf1, 0xe8, 0xff },
	SDL_Color { 0xff, 0x00, 0x4d, 0xff },
	SDL_Color { 0xff, 0xa3, 0x00, 0xff },
	SDL_Color { 0xff, 0xec, 0x27, 0xff },
	SDL_Color { 0x00, 0xe4, 0x36, 0xff },
	SDL_Color { 0x29, 0xad, 0xff, 0xff },
	SDL_Color { 0x83, 0x76, 0x9c, 0xff },
	SDL_Color { 0xff, 0x77, 0xa8, 0xff },
	SDL_Color { 0xff, 0xcc, 0xaa, 0xff },
	SDL_Color { 0xff, 0x00, 0x00, 0xff },
};

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* spritesheet;

bool init_renderer();
bool renderer_wait_until_ready();
void clear_screen();
void load_spritesheet(std::vector<unsigned char>& sprite_map);
void draw_sprite(int n, int x, int y, int w, int h);
void set_color(int col);
void clip_outside();