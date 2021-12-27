#include <iostream>
#include <SDL.h>
#include <SDL_image.h>


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

class Player {
private:
	const int w = 128;
	const int h = 128;
	int x, y;
	SDL_Surface* img;
	const int moveAmount = 20;

public:
	Player() : x(SCREEN_WIDTH / 2), y(SCREEN_HEIGHT - h) {
		img = IMG_Load("resources\\player.png");
		if (!img) {
			std::cout << "Failed to load resources\\player.png: " << IMG_GetError() << std::endl;
		}
	}

	void moveRight() {
		x += moveAmount;
	}

	void moveLeft() {
		x -= moveAmount;
	}

	void draw(SDL_Window* window, SDL_Surface* winSurface) {
		SDL_Rect dest;
		dest.x = this->x;
		dest.y = this->y;
		SDL_BlitSurface(this->img, nullptr, winSurface, &dest);
		SDL_UpdateWindowSurface(window);
	}
};


void clearWindow(SDL_Window* window, SDL_Surface* winSurface) {
	SDL_FillRect(winSurface, nullptr, SDL_MapRGB(winSurface->format, 0, 0, 0));
	SDL_UpdateWindowSurface(window);
}

int main(int argc, char** args) {

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

	Player player;
	player.draw(window, winSurface);

	SDL_Event ev;
	while (true) {
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym) {
				case SDLK_LEFT:
					player.moveLeft();
					clearWindow(window, winSurface);
					player.draw(window, winSurface);
					break;
				case SDLK_RIGHT:
					player.moveRight();
					clearWindow(window, winSurface);
					player.draw(window, winSurface);
					break;
				}
				break;
			}
		}
	}

	system("pause");
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}