#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

const int SCREEN_WIDTH = 2048;
const int SCREEN_HEIGHT = 1024;


enum Direction {
	Left,
	Right
};


class Alien {
private:
	SDL_Surface* img1;
	SDL_Surface* img2;
	SDL_Surface* currentImg;

public:
	int x, y;
	static const int w = 128;
	static const int h = 96;
	static const int moveAmount = 20;

public:
	Alien(SDL_Surface* img1, SDL_Surface* img2, int x, int y)
		: img1(img1), img2(img2), x(x), y(y) {
		this->currentImg = img1;
	}

	void toggleImage() {
		this->currentImg = currentImg == img1 ? img2 : img1;
	}

	void moveRight() {
		this->x += moveAmount;
		this->currentImg = currentImg == img1 ? img2 : img1;
	}

	void moveLeft() {
		this->x -= moveAmount;
		this->currentImg = currentImg == img1 ? img2 : img1;
	}

	void moveDown() {
		this->y += moveAmount;
		this->currentImg = currentImg == img1 ? img2 : img1;
	}

	void draw(SDL_Window* window, SDL_Surface* winSurface) {
		SDL_Rect dest;
		dest.x = this->x;
		dest.y = this->y;
		SDL_BlitSurface(this->currentImg, nullptr, winSurface, &dest);
		SDL_UpdateWindowSurface(window);
	}
};


class MoveDirection {
protected:
	Alien* alien;
	int moveAmount;

public:
	virtual bool move(const int moveAmount) = 0;
	virtual void undoMove() = 0;

	virtual void setAlien(Alien* alien) {
		this->alien = alien;
	}
};


class MoveLeft : public MoveDirection {
public:
	virtual bool move(const int moveAmount) override {
		if (alien->x - moveAmount < 0) {
			return false;
		}
		else {
			this->moveAmount = moveAmount;
			alien->x -= moveAmount;
			alien->toggleImage();
			return true;
		}
	}

	virtual void undoMove() override {
		alien->x += this->moveAmount;
		alien->toggleImage();
	}
};


class MoveRight : public MoveDirection {
public:
	virtual bool move(const int moveAmount) override {
		if (alien->x + Alien::w + moveAmount > SCREEN_WIDTH) {
			return false;
		}
		else {
			this->moveAmount = moveAmount;
			alien->x += moveAmount;
			alien->toggleImage();
			return true;
		}
	}

	virtual void undoMove() override {
		alien->x -= this->moveAmount;
		alien->toggleImage();
	}
};


class MoveDown : public MoveDirection {
public:
	virtual bool move(const int moveAmount) override {
		if (alien->y + Alien::h + moveAmount > SCREEN_HEIGHT) {
			return false;
		}
		else {
			this->moveAmount = moveAmount;
			alien->y += moveAmount;
			alien->toggleImage();
			return true;
		}
	}

	virtual void undoMove() override {
		alien->y -= this->moveAmount;
		alien->toggleImage();
	}
};


class Aliens {
private:
	const int numRows = 5;
	const int numCols = 11;
	int moveIdx = 0;
	MoveLeft leftMover;
	MoveRight rightMover;
	MoveDown downMover;
	MoveDirection* movers[4] = { &rightMover, &downMover, &leftMover, &downMover };
	std::vector<Alien> aliens;

public:
	Aliens() {
		SDL_Surface* img1a = IMG_Load("resources\\alien1_a.png");
		SDL_Surface* img1b = IMG_Load("resources\\alien1_b.png");
		SDL_Surface* img2a = IMG_Load("resources\\alien2_a.png");
		SDL_Surface* img2b = IMG_Load("resources\\alien2_b.png");
		SDL_Surface* img3a = IMG_Load("resources\\alien3_a.png");
		SDL_Surface* img3b = IMG_Load("resources\\alien3_b.png");

		for (int i = 0; i < numCols; i++) {
			aliens.push_back(Alien(img1a, img1b, i * Alien::w, 0));
		}
		for (int i = 0; i < numCols; i++) {
			aliens.push_back(Alien(img2a, img2b, i * Alien::w, Alien::h));
		}
		for (int i = 0; i < numCols; i++) {
			aliens.push_back(Alien(img2a, img2b, i * Alien::w, 2 * Alien::h));
		}
		for (int i = 0; i < numCols; i++) {
			aliens.push_back(Alien(img3a, img3b, i * Alien::w, 3 * Alien::h));
		}
		for (int i = 0; i < numCols; i++) {
			aliens.push_back(Alien(img3a, img3b, i * Alien::w, 4 * Alien::h));
		}
	}

	bool move() {
		for (int i = 0; i < 2; i++) {
			bool moveSuccess;
			MoveDirection* mover = movers[moveIdx];
			for (int i = 0; i < aliens.size(); i++) {
				mover->setAlien(&aliens[i]);
				moveSuccess = mover->move(Alien::moveAmount);
				if (!moveSuccess) {
					for (int j = 0; j < i; j++) {
						mover->setAlien(&aliens[j]);
						mover->undoMove();
					}
					break;
				}
			}
			if (moveSuccess) {
				if (moveIdx % 2 == 1) {
					moveIdx++;
					moveIdx %= 4;
				}
				return true;
			}
			else {
				moveIdx++;
			}
		}
		return false;
	}

	void draw(SDL_Window* window, SDL_Surface* winSurface) {
		for (Alien& alien : aliens) {
			alien.draw(window, winSurface);
		}
	}
};


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

	Aliens aliens;
	aliens.draw(window, winSurface);

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
		aliens.move();
		SDL_Delay(500);
		clearWindow(window, winSurface);
		aliens.draw(window, winSurface);
		player.draw(window, winSurface);
	}

	system("pause");
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}