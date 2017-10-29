#include <iostream>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CHUNK_SIZE = 10;
const int DOT_COUNT = 20;

struct Dot {
	float x;
	float y;
	float xvel;
	float yvel;
	int color;
};

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

Dot *dots = new Dot[DOT_COUNT];
Uint32 *pixels = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];

Dot* find_closest_dot(float x, float y);

void render() {
	SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(Uint32));

	Dot *closest_dot;
	Dot *ptr;
	int dx;
	int dy;
	int index;
	int color;
	int x_chunks = ceil(SCREEN_WIDTH / CHUNK_SIZE);
	int y_chunks = ceil(SCREEN_HEIGHT / CHUNK_SIZE);

	for(int cx = 0; cx < x_chunks; cx++) {
		for(int cy = 0; cy < y_chunks; cy++) {
			float x = (float) (cx * CHUNK_SIZE);
			float y = (float) (cy * CHUNK_SIZE);

			Dot *tl = find_closest_dot(x, y);
			Dot *tr = find_closest_dot(x + CHUNK_SIZE - 1, y);
			Dot *bl = find_closest_dot(x, y + CHUNK_SIZE - 1);
			Dot *br = find_closest_dot(x + CHUNK_SIZE - 1, y + CHUNK_SIZE - 1);

			bool full = (tl == tr && tl == bl && tl == br);
			if (full) closest_dot = tl;

			for(float dx = 0; dx < CHUNK_SIZE; dx++) {
				for(float dy = 0; dy < CHUNK_SIZE; dy++) {
					index = (((y + dy) * SCREEN_WIDTH) + (x + dx));
					if (full) {
						pixels[index] = closest_dot->color;
					} else {
						closest_dot = find_closest_dot(x + dx, y + dy);
						pixels[index] = closest_dot->color;
					}
				}
			}
		}
	}

	ptr = dots;
	for(int i = 0; i < DOT_COUNT; i++, ptr++) {
		dx = (int) ptr->x;
		dy = (int) ptr->y;
		pixels[((dy * SCREEN_WIDTH) + dx)] = 0;
		pixels[(((dy + 1) * SCREEN_WIDTH) + dx)] = 0;
		pixels[((dy * SCREEN_WIDTH) + (dx + 1))] = 0;
		pixels[(((dy + 1) * SCREEN_WIDTH) + (dx + 1))] = 0;
	}

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

Dot* find_closest_dot(float x, float y) {
	Dot *ptr;
	ptr = dots;
	Dot *closest_dot;
	float closest_dis = 1000000;
	float dis;

	for(int i = 0; i < DOT_COUNT; i++, ptr++) {
		int dx = (int) ptr->x;
		int dy = (int) ptr->y;
		dis = (float) (((dx - x) * (dx - x)) + ((dy - y) * (dy  - y)));
		if(dis < closest_dis) {
			closest_dot = ptr;
			closest_dis = dis;
		}
	}

	return closest_dot;
}

void run() {
	Dot *ptr;
	ptr = dots;

	for(int i = 0; i < DOT_COUNT; i++, ptr++) {
		ptr->x += ptr->xvel;
		ptr->y += ptr->yvel;

		if(ptr->x <= 1 || ptr->x >= SCREEN_WIDTH - 1) {
			if(ptr->x <= 1) ptr->x = 2;
			if(ptr->x >= SCREEN_WIDTH - 1) ptr->x = SCREEN_WIDTH - 2;
			ptr->xvel *= -1;
		}

		if(ptr->y <= 1 || ptr->y >= SCREEN_HEIGHT - 1) {
			if(ptr->y <= 1) ptr->y = 2;
			if(ptr->y >= SCREEN_WIDTH - 1) ptr->y = SCREEN_WIDTH - 2;
			ptr->yvel *= -1;
		}

	}
}

int main(int argc, char* args[]) {
	bool quit = false;
	SDL_Event event;

	srand(time(NULL));

	memset(pixels, 255, 640 * 480 * sizeof(Uint32));

	window = SDL_CreateWindow("ボロノイ図は最良です。",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	renderer = SDL_CreateRenderer(window, -1, 0);

	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

	for(int i = 0; i < DOT_COUNT; i++) {
		dots[i].x = (rand() % (SCREEN_WIDTH - 4)) + 2;
		dots[i].y = (rand() % (SCREEN_HEIGHT - 4)) + 2;
		dots[i].xvel = (((float) (rand() % 6)) / 3.0) - 1;
		dots[i].yvel = (((float) (rand() % 6)) / 3.0) - 1;
		dots[i].color = ((rand() % 256) << 16) + ((rand() % 256) << 8) + ((rand() % 256) << 0) + (255 << 24);
	}

	while(!quit) {
		SDL_PollEvent(&event);

		switch(event.type) {
			case SDL_QUIT:
				quit = true;
				break;
		}

		render();
		run();
	}

	delete[] pixels;

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
