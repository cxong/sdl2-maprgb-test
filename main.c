#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>

#define SCREEN_W 320
#define SCREEN_H 240


int main(int argc, char *argv[])
{
	Uint32 *buf = NULL;
	SDL_Texture *t = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		goto bail;
	}

	// Create surfaces
	buf = malloc(SCREEN_W * SCREEN_H * sizeof(Uint32));
	if (buf == NULL)
	{
		printf("Failed to allocate buffer\n");
		goto bail;
	}

	// Create display window based on image size
	window = SDL_CreateWindow(
		argv[1], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_W, SCREEN_H, 0);
	if (window == NULL)
	{
		printf("Failed to create window: %s\n", SDL_GetError());
		goto bail;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		goto bail;
	}
	SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);

	// Create texture
	t = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		SCREEN_W, SCREEN_H);
	if (t == NULL)
	{
		printf("Failed to create screen texture: %s\n", SDL_GetError());
		goto bail;
	}

	// Render to texture
	SDL_UpdateTexture(t, NULL, buf, SCREEN_W * sizeof(Uint32));
	if (SDL_RenderClear(renderer) != 0)
	{
		printf("Failed to clear renderer: %s\n", SDL_GetError());
		goto bail;
	}

	// Blit the texture to screen
	if (SDL_RenderCopy(renderer, t, NULL, NULL) != 0)
	{
		printf("Failed to blit surface: %s\n", SDL_GetError());
		goto bail;
	}

	// Display
	SDL_RenderPresent(renderer);

	// Wait for keypress to exit
	bool quit = false;
	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYDOWN || e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		SDL_Delay(100);
	}


bail:
	SDL_DestroyTexture(t);
	free(buf);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
