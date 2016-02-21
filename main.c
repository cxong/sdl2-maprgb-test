#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>

#define SCREEN_W 320
#define SCREEN_H 240


int main(int argc, char *argv[])
{
	Uint32 *buf = NULL;
	SDL_Texture *t = NULL;
	SDL_PixelFormat *format = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		goto bail;
	}

	// Create surface
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
	format = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));
	if (format == NULL)
	{
		printf("Failed to alloc pixel format: %s\n", SDL_GetError());
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

	// Draw a palette on the buffer
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define CLAMP(v, _min, _max) MAX((_min), MIN((_max), (v)))
#define GREY_MARGIN 20
	for (int y = 0; y < SCREEN_H; y++)
	{
		Uint8 v = (Uint8)CLAMP((SCREEN_H - y) * 255.0 / (SCREEN_H / 2), 0, 255);
		double s = CLAMP(y * 1.0 / (SCREEN_H / 2), 0.0, 1.0);
		for (int x = 0; x < SCREEN_W; x++)
		{
			SDL_Color c;
			c.a = 255;
			if (x < SCREEN_W - GREY_MARGIN)
			{
				double h = x * 360.0 / (SCREEN_W - GREY_MARGIN);
				double ff;
				Uint8 p, q, t;
				long i;
				double hh = h;
				if (hh >= 360.0)
				{
					hh = 0.0;
				}
				hh /= 60.0;
				i = (long)hh;
				ff = hh - i;
				p = (Uint8)CLAMP(v * (1.0 - s), 0, 255);
				q = (Uint8)CLAMP(v * (1.0 - (s * ff)), 0, 255);
				t = (Uint8)CLAMP(v * (1.0 - (s * (1.0 - ff))), 0, 255);

				switch (i)
				{
				case 0:
					c.r = v;
					c.g = t;
					c.b = p;
					break;
				case 1:
					c.r = q;
					c.g = v;
					c.b = p;
					break;
				case 2:
					c.r = p;
					c.g = v;
					c.b = t;
					break;

				case 3:
					c.r = p;
					c.g = q;
					c.b = v;
					break;
				case 4:
					c.r = t;
					c.g = p;
					c.b = v;
					break;
				case 5:
				default:
					c.r = v;
					c.g = p;
					c.b = q;
					break;
				}
			}
			else
			{
				c.r = c.g = c.b = (Uint8)CLAMP((SCREEN_H - y) * 255.0 / SCREEN_H, 0, 255);
			}
			Uint32 pixel = SDL_MapRGBA(format, c.r, c.g, c.b, c.a);
			buf[x + y*SCREEN_W] = pixel;
		}
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
	SDL_FreeFormat(format);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
