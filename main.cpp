#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

int main(int argc, char** args) {
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

	SDL_Surface* winSurface = nullptr;
	SDL_Window* window = nullptr;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Error initialising SDL: " << SDL_GetError() << std::endl;
		system("pause");
		return 1;
	}

	int result = IMG_Init(IMG_INIT_PNG);

	if (!result) {
		std::cout << "Failed to initialise SDL_image: " << IMG_GetError() << std::endl;
	}

	window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!window) {
		std::cout << "Error creating window: " << SDL_GetError() << std::endl;
		system("pause");
		return 1;
	}

	winSurface = SDL_GetWindowSurface(window);

	if (!winSurface) {
		std::cout << "Error getting surface: " << SDL_GetError() << std::endl;
		system("pause");
		return 1;
	}

	SDL_FillRect(winSurface, nullptr, SDL_MapRGB(winSurface->format, 0, 0, 0));
	SDL_UpdateWindowSurface(window);

	SDL_Surface* img = IMG_Load("resources\\player.png");

	if (!img) {
		std::cout << "Failed to load resources\\player.png: " << IMG_GetError() << std::endl;
	}

	SDL_Rect dest;
	dest.x = 128;
	dest.y = 128;
	SDL_BlitSurface(img, nullptr, winSurface, &dest);
	SDL_UpdateWindowSurface(window);

	system("pause");
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}