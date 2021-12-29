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


struct BoxCoords {
	int x, y, w, h;
};


class Bullet {
private:
	SDL_Surface* img;
	const int w = 128;
	const int h = 128;
	const int x;
	int y;
	BoxCoords dynamicBox;

public:
	Bullet(SDL_Surface* img, int x) : img(img), x(x), y(SCREEN_HEIGHT - h) {
		dynamicBox.x = x + w / 2;
		dynamicBox.y = y + h / 4;
		dynamicBox.w = 8;
		dynamicBox.h = 24;
	}

	BoxCoords& getDynamicBox() {
		return this->dynamicBox;
	}

	void move(int moveAmount) {
		this->y += moveAmount;
		dynamicBox.y += moveAmount;
	}

	/*
	void attemptDestroy(std::vector<Alien>& aliens) {
		BoxCoords& bulletDynamicBox = this->dynamicBox;
		for (Alien& alien : aliens) {
			BoxCoords& alienDynamicBox = alien.getDynamicBox();
			if ((bulletDynamicBox.x > alienDynamicBox.x)
				&& (bulletDynamicBox.x < (alienDynamicBox.x + alienDynamicBox.w))
				&& (bulletDynamicBox.y > alienDynamicBox.y)
				&& (bulletDynamicBox.y < (alienDynamicBox.y + alienDynamicBox.h))) {
				alien.setDestroyed();
			}
		}
	}
	*/

	void draw(SDL_Surface* winSurface) {
		SDL_Rect dest;
		dest.x = this->x;
		dest.y = this->y;
		SDL_BlitSurface(this->img, nullptr, winSurface, &dest);
	}
};


class Alien {
private:
	SDL_Surface* img1;
	SDL_Surface* img2;
	SDL_Surface* imgDestroyed;
	SDL_Surface* currentImg;
	BoxCoords dynamicBox;
	bool destroyed = false;

public:
	int x, y;
	static const int w = 128;
	static const int h = 96;

public:
	Alien(SDL_Surface* img1, SDL_Surface* img2, int x, int y)
		: img1(img1), img2(img2), x(x), y(y) {
		dynamicBox.x = x + w / 5;
		dynamicBox.y = y + h / 5;
		dynamicBox.w = 3 * w / 5;
		dynamicBox.h = 3 * h / 5;
		this->imgDestroyed = IMG_Load("resources\\alien_explosion.png");
		this->currentImg = img1;
	}

	BoxCoords& getDynamicBox() {
		return this->dynamicBox;
	}

	void setDestroyed() {
		this->destroyed = true;
		toggleImage();
	}

	void toggleImage() {
		if (this->destroyed) {
			this->currentImg = imgDestroyed;
		}
		else {
			this->currentImg = currentImg == img1 ? img2 : img1;
		}
	}

	void moveRight(int moveAmount) {
		dynamicBox.x += moveAmount;
		this->x += moveAmount;
		toggleImage();
	}

	void moveLeft(int moveAmount) {
		dynamicBox.x -= moveAmount;
		this->x -= moveAmount;
		toggleImage();
	}

	void moveDown(int moveAmount) {
		dynamicBox.y += moveAmount;
		this->y += moveAmount;
		toggleImage();
	}

	void moveUp(int moveAmount) {
		dynamicBox.y -= moveAmount;
		this->y -= moveAmount;
		toggleImage();
	}

	void draw(SDL_Window* window, SDL_Surface* winSurface) {
		SDL_Rect dest;
		dest.x = this->x;
		dest.y = this->y;
		SDL_BlitSurface(this->currentImg, nullptr, winSurface, &dest);
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
			alien->moveLeft(moveAmount);
			/*
			this->moveAmount = moveAmount;
			alien->x -= moveAmount;
			alien->toggleImage();
			*/
			return true;
		}
	}

	virtual void undoMove() override {
		alien->moveRight(this->moveAmount);
		/*
		alien->x += this->moveAmount;
		alien->toggleImage();
		*/
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
			alien->moveRight(moveAmount);
			/*
			alien->x += moveAmount;
			alien->toggleImage();
			*/
			return true;
		}
	}

	virtual void undoMove() override {
		alien->moveLeft(this->moveAmount);
		/*
		alien->x -= this->moveAmount;
		alien->toggleImage();
		*/
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
			alien->moveDown(moveAmount);
			/*
			alien->y += moveAmount;
			alien->toggleImage();
			*/
			return true;
		}
	}

	virtual void undoMove() override {
		alien->moveUp(this->moveAmount);
		/*
		alien->y -= this->moveAmount;
		alien->toggleImage();
		*/
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

	std::vector<Alien>& getAliens() {
		return this->aliens;
	}

	bool move() {
		for (int i = 0; i < 2; i++) {
			bool moveSuccess;
			MoveDirection* mover = movers[moveIdx];
			for (int i = 0; i < aliens.size(); i++) {
				mover->setAlien(&aliens[i]);
				moveSuccess = mover->move(20);
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
	Bullet* bullet = nullptr;

public:
	Player() : x(SCREEN_WIDTH / 2), y(SCREEN_HEIGHT - h) {
		img = IMG_Load("resources\\player.png");
		if (!img) {
			std::cout << "Failed to load resources\\player.png: " << IMG_GetError() << std::endl;
		}
	}

	Bullet* shoot() {
		if (this->bullet == nullptr) {
			SDL_Surface* img = IMG_Load("resources\\player_bullet.png");
			this->bullet = new Bullet(img, this->x);
		}
		return this->bullet;
	}

	Bullet* getBullet() {
		return this->bullet;
	}

	void destroyBullet() {
		delete bullet;
		this->bullet = nullptr;
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
	}
};


bool attemptDestroyAliens(Player& player, Aliens& aliensObj) {
	Bullet* bullet = player.getBullet();
	std::vector<Alien>& aliens = aliensObj.getAliens();
	BoxCoords& bulletDynamicBox = bullet->getDynamicBox();
	for (int i = 0; i < aliens.size(); i++) {
		BoxCoords& alienDynamicBox = aliens[i].getDynamicBox();
		if ((bulletDynamicBox.x > alienDynamicBox.x)
			&& (bulletDynamicBox.x < (alienDynamicBox.x + alienDynamicBox.w))
			&& (bulletDynamicBox.y > alienDynamicBox.y)
			&& (bulletDynamicBox.y < (alienDynamicBox.y + alienDynamicBox.h))) {
			aliens.erase(aliens.begin() + i);
			player.destroyBullet();
			return true;
		}
	}
	return false;
}

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

	//Bullet* bullet = nullptr;

	SDL_Event ev;
	unsigned int startTime = SDL_GetTicks();
	unsigned int startTime1 = startTime;
	while (true) {
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym) {
				case SDLK_LEFT:
					player.moveLeft();
					clearWindow(window, winSurface);
					player.draw(window, winSurface);
					aliens.draw(window, winSurface);
					SDL_UpdateWindowSurface(window);
					break;
				case SDLK_RIGHT:
					player.moveRight();
					clearWindow(window, winSurface);
					player.draw(window, winSurface);
					aliens.draw(window, winSurface);
					SDL_UpdateWindowSurface(window);
					break;
				case SDLK_SPACE:
					player.shoot();
					break;
				}
				break;
			}
		}

		if (player.getBullet()) {
			BoxCoords& bulletDynamicBox = player.getBullet()->getDynamicBox();
			if ((bulletDynamicBox.y + bulletDynamicBox.h) < 0) {
				player.destroyBullet();
			}
		}

		if (player.getBullet()) {
			if (attemptDestroyAliens(player, aliens)) {
				clearWindow(window, winSurface);
				aliens.draw(window, winSurface);
				player.draw(window, winSurface);
				SDL_UpdateWindowSurface(window);
			}
		}

		unsigned int endTime = SDL_GetTicks();
		if (player.getBullet() && (endTime - startTime1 > 50)) {
			startTime1 = endTime;
			player.getBullet()->move(-20);
			clearWindow(window, winSurface);
			player.getBullet()->draw(winSurface);
			aliens.draw(window, winSurface);
			player.draw(window, winSurface);
			SDL_UpdateWindowSurface(window);
		}
		if (endTime - startTime > 500) {
			startTime = endTime;
			aliens.move();
			clearWindow(window, winSurface);
			aliens.draw(window, winSurface);
			player.draw(window, winSurface);
			if (player.getBullet()) {
				player.getBullet()->draw(winSurface);
			}
			SDL_UpdateWindowSurface(window);
		}
	}

	system("pause");
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}