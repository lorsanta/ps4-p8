﻿#include "renderer.h"
#include <vector>
#include "memory.h"
#include "log.h"
#include <thread>
#include "efla.e.h"
#include <math.h>

std::vector<unsigned char> transform_spritesheet_data(std::vector<unsigned char>& input);

#define DEBUGLOG Renderer_DEBUGLOG
Log DEBUGLOG = logger.log("renderer");

SDL_Rect p8_area {
	0,0,P8_WIDTH,P8_HEIGHT
};

SDL_Color DEFAULT_PALETTE[] = {
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
};
SDL_Color EXTENDED_PALETTE[] = {
	SDL_Color { 0x29, 0x18, 0x14, 0xff },
	SDL_Color { 0x11, 0x1D, 0x35, 0xff },
	SDL_Color { 0x42, 0x21, 0x36, 0xff },
	SDL_Color { 0x12, 0x53, 0x59, 0xff },
	SDL_Color { 0x74, 0x2F, 0x29, 0xff },
	SDL_Color { 0x49, 0x33, 0x3B, 0xff },
	SDL_Color { 0xA2, 0x88, 0x79, 0xff },
	SDL_Color { 0xF3, 0xEF, 0x7D, 0xff },
	SDL_Color { 0xBE, 0x12, 0x50, 0xff },
	SDL_Color { 0xFF, 0x6C, 0x24, 0xff },
	SDL_Color { 0xA8, 0xE7, 0x2E, 0xff },
	SDL_Color { 0x00, 0xB5, 0x43, 0xff },
	SDL_Color { 0x06, 0x5A, 0xB5, 0xff },
	SDL_Color { 0x75, 0x46, 0x65, 0xff },
	SDL_Color { 0xFF, 0x6E, 0x59, 0xff },
	SDL_Color { 0xFF, 0x9D, 0x81, 0xff },
};

#define SPRITESHEET_LENGTH 0x2000
std::vector<unsigned char> transform_spritesheet_data(std::vector<unsigned char>& input)
{
	// TODO palt to change transparency
	std::vector<unsigned char> ret(SPRITESHEET_LENGTH * 2 * 4); // Every byte encodes two pixels, each pixel is 4 bytes in RGBA

	int p = 0;
	for (int i = 0; i < SPRITESHEET_LENGTH; i++) {
		unsigned char left = input[i] & 0x0F;
		if (left == 0) {
			ret[p++] = 0x00;
			ret[p++] = 0x00;
			ret[p++] = 0x00;
			ret[p++] = 0x00;
		}
		else {
			SDL_Color c = DEFAULT_PALETTE[left];
			ret[p++] = c.a;
			ret[p++] = c.b;
			ret[p++] = c.g;
			ret[p++] = c.r;
		}

		unsigned char right = input[i] >> 4;
		if (right == 0) {
			ret[p++] = 0x00;
			ret[p++] = 0x00;
			ret[p++] = 0x00;
			ret[p++] = 0x00;
		}
		else {
			SDL_Color c = DEFAULT_PALETTE[right];
			ret[p++] = c.a;
			ret[p++] = c.b;
			ret[p++] = c.g;
			ret[p++] = c.r;
		}
	}

	return ret;
}

Renderer::Renderer()
{
	// Initialize SDL functions
	DEBUGLOG << "Initializing SDL" << ENDL;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) != 0)
	{
		DEBUGLOG << "Failed to initialize SDL: " << SDL_GetError() << ENDL;
		exit(1);
	}

	// Create a window context
	DEBUGLOG << "Creating a window" << ENDL;

	this->window = SDL_CreateWindow("main", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, FRAME_WIDTH, FRAME_HEIGHT, 0);

	if (!this->window)
	{
		DEBUGLOG << "Failed to create window: " << SDL_GetError() << ENDL;
		exit(1);
	}

	// Create a software rendering instance for the window
	SDL_Surface* windowSurface = SDL_GetWindowSurface(this->window);
	this->renderer = SDL_CreateSoftwareRenderer(windowSurface);

	if (!this->renderer)
	{
		DEBUGLOG << "Failed to create software renderer: " << SDL_GetError() << ENDL;
		SDL_Quit();
		exit(1);
	}
}

void Renderer::initialize()
{
	// If we make a logical size of P8_HEIGHT, then SDL struggles with decimal scales and adds random black lines.
	// So we need a logical size that divides P8_HEIGHT
	unsigned int scale = FRAME_HEIGHT / P8_HEIGHT;
	// Assuming square, but logic should still work for other aspect ratios (but more complex)
	unsigned int logicalSize = FRAME_HEIGHT / scale;
	SDL_RenderSetLogicalSize(this->renderer, logicalSize, logicalSize);

	// Center the viewport as much as posible
	SDL_Rect viewport{};
	SDL_RenderGetViewport(this->renderer, &viewport);
	viewport.y = (viewport.h - P8_HEIGHT) / 2;
	SDL_RenderSetViewport(this->renderer, &viewport);

	SDL_SetRenderDrawColor(this->renderer, 0x10, 0x10, 0x10, 0xFF);
	SDL_RenderClear(this->renderer);

	// Inside area
	SDL_SetRenderDrawColor(this->renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(this->renderer, &p8_area);
	SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_UpdateWindowSurface(this->window);

	this->reset_draw_pal();
	this->reset_screen_pal();
	memset(this->prev_screen, 0x00, SCREEN_MEMORY_SIZE);

	p8_memory[ADDR_DS_CLIP_RECT] = 0;
	p8_memory[ADDR_DS_CLIP_RECT + 1] = 0;
	p8_memory[ADDR_DS_CLIP_RECT + 2] = 127;
	p8_memory[ADDR_DS_CLIP_RECT + 3] = 127;

	p8_memory[ADDR_DS_COLOR] = 6;

	memory_write_short(ADDR_DS_CAMERA_X, 0);
	memory_write_short(ADDR_DS_CAMERA_Y, 0);
}

void Renderer::clear_screen(unsigned char color)
{
	// Every byte in the screen data is two pixels, we want to color the both of them with that color.
	color = (color & 0x0F) | (color << 4);
	memset(&p8_memory[ADDR_SCREEN], color, 0x2000);
}

// Screen cooridantes (after camera transform!)
bool is_y_drawable(int y) {
	unsigned char y0 = p8_memory[ADDR_DS_CLIP_RECT + 1];
	unsigned char y1 = p8_memory[ADDR_DS_CLIP_RECT + 3];
	return y0 <= y && y <= y1;
}
bool is_x_drawable(int x) {
	unsigned char x0 = p8_memory[ADDR_DS_CLIP_RECT];
	unsigned char x1 = p8_memory[ADDR_DS_CLIP_RECT + 2];
	return (x0 <= x && x <= x1);
}
bool is_drawable(int x, int y) {
	return is_x_drawable(x) && is_y_drawable(y);
}

void Renderer::draw_map(int cx, int cy, int sx, int sy, int cw, int ch, unsigned char layer)
{
	for (int y = 0; y < ch; y++) {
		int row = cy + y;
		int row_offset = ADDR_MAP + row * 128;
		if (row >= 32) {
			row_offset = ADDR_MAP_SHARED + (row-32) * 128;
		}
		int y_pos = sy + y * 8;
		int screen_y = y_pos - (short)memory_read_short(ADDR_DS_CAMERA_Y);
		if (!is_y_drawable(screen_y) && !is_y_drawable(screen_y + 8)) {
			continue;
		}

		for (int x = 0; x < cw; x++) {
			int col = cx + x;
			if (col > 128) {
				continue;
			}

			int n = p8_memory[row_offset + col];
			if (n == 0) {
				continue;
			}
			int x_pos = sx + x * 8;

			unsigned char flags = p8_memory[ADDR_SPRITE_FLAGS + n];
			bool passes_filter = (unsigned char)(~layer | flags) == 0xFF;
			if (passes_filter) {
				this->draw_sprite(n, x_pos, y_pos, 8, 8, false, false);
			}
		}
	}
}

#define LINE_JMP 128 / 2
void Renderer::draw_sprite(int n, int x, int y, int w, int h, bool flip_x, bool flip_y)
{
	int sprite_col = n % 16;
	int sprite_row = n / 16;

	this->draw_from_spritesheet(sprite_col * 8, sprite_row * 8, w, h, x, y, w, h, flip_x, flip_y);
}

void Renderer::draw_from_spritesheet(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, bool flip_x, bool flip_y)
{
	if (!this->will_be_drawn(dx, dy, dw, dh)) {
		return;
	}

	int sprite_addr = ADDR_SPRITE_SHEET + sy * LINE_JMP + sx / 2;
	for (int _y = 0; _y < dh; _y++) {
		int sprite_y = _y * sh / dh;
		if (flip_y) {
			sprite_y = sh - sprite_y;
		}
		for (int _x = 0; _x < dw; _x++) {
			int sprite_x = _x * sw / dw;
			if (flip_x) {
				sprite_x = sw - sprite_x - 1;
			}

			unsigned char color = p8_memory[sprite_addr + sprite_y * LINE_JMP + sprite_x / 2];
			if (sprite_x % 2 == 0) {
				color = color & 0x0F;
			}
			else {
				color = color >> 4;
			}
			this->set_transform_pixel(dx + _x, dy + _y, color, true);
		}
	}
}

void Renderer::draw_point(int x, int y)
{
	unsigned char color = p8_memory[ADDR_DS_COLOR] & 0x0F;
	this->set_transform_pixel(x, y, color, false);
}

void Renderer::draw_points(std::vector<Renderer_Point>& points)
{
	for (int i = 0; i < points.size(); i++) {
		this->draw_point(points[i].x, points[i].y);
	}
}

void Renderer::draw_line(int x0, int y0, int x1, int y1)
{
	std::vector<Renderer_Point> points = efla_small_line(x0, y0, x1, y1);
	this->draw_points(points);
}

// TODO fill + pattern
void Renderer::draw_oval(int x0, int y0, int x1, int y1, bool fill)
{
	int width = x1 - x0;
	int height = y1 - y0;
	double mid_x = x0 + (double)width / 2;
	double mid_y = y0 + (double)height / 2;

	// The second part of this formula was taken temptatively.... halfway_t is cos(M_PI/4) for circles, but not for ellipses where they are squished
	// On the limit when width or height = 0, then it needs to go all the way from 0..1. So this seems to work.
	double halfway_t = cos(M_PI / 4 - (double)abs(width-height) / std::max(width, height) * M_PI / 4);
	int halfway_h = ceil(halfway_t * height / 2);
	for (int dy = 0; dy <= halfway_h; dy++) {
		double y_t = 2 * (double)dy / height; // dy/(height/2) = 2*dy/height
		double x_t = sqrt(1 - y_t * y_t);

		int x2 = round(mid_x + x_t * width / 2);
		int x3 = round(mid_x - x_t * width / 2);
		int y2 = round(mid_y + dy);
		int y3 = round(mid_y - dy);
		this->draw_point(x2, y2);
		this->draw_point(x2, y3);
		this->draw_point(x3, y2);
		this->draw_point(x3, y3);
	}
	
	int halfway_w = ceil(halfway_t * width / 2);
	for (int dx = 0; dx <= halfway_w; dx++) {
		double x_t = 2 * (double)dx / width;
		double y_t = sqrt(1 - x_t * x_t);

		int x2 = round(mid_x + x_t * width / 2);
		int x3 = round(mid_x - x_t * width / 2);
		int y2 = round(mid_y + y_t * height / 2);
		int y3 = round(mid_y - y_t * height / 2);
		this->draw_point(x2, y2);
		this->draw_point(x2, y3);
		this->draw_point(x3, y2);
		this->draw_point(x3, y3);
	}
}

void Renderer::draw_rectangle(int x0, int y0, int x1, int y1, bool fill)
{
	unsigned char color = p8_memory[ADDR_DS_COLOR] & 0x0F;

	// TODO pattern

	int start_x = x0 - (short)memory_read_short(ADDR_DS_CAMERA_X);
	int end_x = x1 - (short)memory_read_short(ADDR_DS_CAMERA_X);
	int start_y = y0 - (short)memory_read_short(ADDR_DS_CAMERA_Y);
	int end_y = y1 - (short)memory_read_short(ADDR_DS_CAMERA_Y);

	if (fill) {
		unsigned char mapped_color = p8_memory[ADDR_DS_DRAW_PAL + color] & 0x0F;
		mapped_color = mapped_color | (mapped_color << 4);
		int clipped_sx = std::max(start_x, (int)p8_memory[ADDR_DS_CLIP_RECT]);
		int clipped_ex = std::min(end_x, (int)p8_memory[ADDR_DS_CLIP_RECT+2]);
		int clipped_sy = std::max(start_y, (int)p8_memory[ADDR_DS_CLIP_RECT+1]);
		int clipped_ey = std::min(end_y, (int)p8_memory[ADDR_DS_CLIP_RECT+3]);

		if (clipped_sx % 2 == 1) {
			for (int y = clipped_sy; y <= clipped_ey; y++) {
				int addr = ADDR_SCREEN + y * LINE_JMP + clipped_sx / 2;
				p8_memory[addr] = (p8_memory[addr] & 0x0F) | (mapped_color << 4);
			}

			clipped_sx++;
		}
		// clipped_sx is now even: we can just fill in pairs
		for (int x = clipped_sx; x < clipped_ex; x+=2) { // Excluding last row: If it ends on a even position, we need to manuall fill in the last column (next for loop)
			for (int y = clipped_sy; y <= clipped_ey; y++) {
				int addr = ADDR_SCREEN + y * LINE_JMP + x / 2;
				p8_memory[addr] = mapped_color;
			}
		}

		if (clipped_ex % 2 == 0) {
			for (int y = clipped_sy; y <= clipped_ey; y++) {
				int addr = ADDR_SCREEN + y * LINE_JMP + clipped_ex / 2;
				p8_memory[addr] = (p8_memory[addr] & 0xF0) | (mapped_color & 0x0F);
			}
		}
	}
	else {
		if (is_y_drawable(start_y) || is_y_drawable(end_y)) {
			for (int x = x0; x <= x1; x++) {
				this->set_transform_pixel(x, y0, color, false);
				this->set_transform_pixel(x, y1, color, false);
			}
		}
		if (is_x_drawable(start_x) || is_x_drawable(start_y)) {
			for (int y = y0; y <= y1; y++) {
				this->set_transform_pixel(x0, y, color, false);
				this->set_transform_pixel(x1, y, color, false);
			}
		}
	}
}

void Renderer::scroll(unsigned char lines)
{
	if (lines >= 128) {
		this->clear_screen(0);
	}
	else {
		unsigned int length = (128 - lines) * LINE_JMP;
		memmove(&p8_memory[ADDR_SCREEN], &p8_memory[ADDR_SCREEN + lines * LINE_JMP], length);
		memset(&p8_memory[ADDR_SCREEN + length], 0, 0x2000 - length);
	}
}

// Canvas cooridantes (without camera transform)
bool Renderer::will_be_drawn(int x, int y, int w, int h)
{
	int screen_x = x - (short)memory_read_short(ADDR_DS_CAMERA_X);
	int screen_y = y - (short)memory_read_short(ADDR_DS_CAMERA_Y);

	return is_drawable(screen_x, screen_y) || is_drawable(screen_x+w, screen_y) || is_drawable(screen_x, screen_y+h) ||is_drawable(screen_x+w, screen_y+h);
}

void Renderer::reset_draw_pal()
{
	char transp_mask = 0x10; // Color 0 = black set to transparent
	for (int i = 0; i < 16; i++) {
		p8_memory[ADDR_DS_DRAW_PAL + i] = i | transp_mask;
		transp_mask = 0; // Only the first mask
	}
}

void Renderer::reset_screen_pal()
{
	for (int i = 0; i < 16; i++) {
		p8_memory[ADDR_DS_SCREEN_PAL + i] = i;
	}
}

void Renderer::set_color_transparent(unsigned char color, bool transparent)
{
	if (transparent) {
		p8_memory[ADDR_DS_DRAW_PAL + color] = p8_memory[ADDR_DS_DRAW_PAL + color] | 0x10;
	} else {
		p8_memory[ADDR_DS_DRAW_PAL + color] = p8_memory[ADDR_DS_DRAW_PAL + color] & 0x0F;
	}
}

void Renderer::reset_transparency_pal()
{
	char transp_mask = 0x10; // Color 0 = black set to transparent
	for (int i = 0; i < 16; i++) {
		p8_memory[ADDR_DS_DRAW_PAL + i] = (p8_memory[ADDR_DS_DRAW_PAL + i] & 0x0F) | transp_mask;
		transp_mask = 0; // Only the first mask
	}
}

void Renderer::present()
{
	for (int i = 0; i < SCREEN_MEMORY_SIZE; i++) {
		if (p8_memory[ADDR_SCREEN + i] == this->prev_screen[i]) {
			continue;
		}

		int x = (i%64)*2;
		int y = i/64;
		unsigned char pixels = p8_memory[ADDR_SCREEN + i];

		unsigned char left = p8_memory[ADDR_DS_SCREEN_PAL + (pixels & 0x0F)];
		SDL_Color leftColor = DEFAULT_PALETTE[left & 0x0F];
		if (left >= 0x10) {
			leftColor = EXTENDED_PALETTE[left & 0x0F];
		}
		SDL_SetRenderDrawColor(this->renderer, leftColor.r, leftColor.g, leftColor.b, 0xFF);
		SDL_RenderDrawPoint(this->renderer, x, y);

		unsigned char right = p8_memory[ADDR_DS_SCREEN_PAL + (pixels >> 4)];
		SDL_Color rightColor = DEFAULT_PALETTE[right & 0x0F];
		if (right >= 0x10) {
			rightColor = EXTENDED_PALETTE[right & 0x0F];
		}
		SDL_SetRenderDrawColor(this->renderer, rightColor.r, rightColor.g, rightColor.b, 0xFF);
		SDL_RenderDrawPoint(this->renderer, x+1, y);
	}
	SDL_UpdateWindowSurface(this->window);
	memcpy(this->prev_screen, &p8_memory[ADDR_SCREEN], SCREEN_MEMORY_SIZE);
}

void Renderer::set_transform_pixel(int x, int y, unsigned char color, bool transparency)
{
	unsigned char mapped_color = p8_memory[ADDR_DS_DRAW_PAL + color];
	// Skip if it's transparent
	if (transparency && mapped_color >= 0x10) {
		return;
	}
	mapped_color = mapped_color & 0x0F;

	int screen_x = x - (short)memory_read_short(ADDR_DS_CAMERA_X);
	int screen_y = y - (short)memory_read_short(ADDR_DS_CAMERA_Y);
	if (is_drawable(screen_x, screen_y)) {
		int addr = ADDR_SCREEN + screen_y * LINE_JMP + screen_x / 2;
		if (screen_x % 2 == 0) {
			p8_memory[addr] = (p8_memory[addr] & 0xF0) | mapped_color;
		}
		else {
			p8_memory[addr] = (p8_memory[addr] & 0x0F) | (mapped_color << 4);
		}
	}
}
