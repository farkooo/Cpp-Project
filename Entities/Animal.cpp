#include "Animal.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
#include <ctime>
#include <cmath>
#include <unordered_map>

struct WolfState {
	double exactX;
	double exactY;
	double angle;
	int turnDir;
};

static std::unordered_map<const Wolf*, WolfState> wolfStates;

Animal::Animal(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: GameObject(r_pGame, r_point, r_width, r_height, GREEN, BLACK)
{
	image_path = img_path;
	curr_pos = r_point;

	do { curr_vel.x = (rand() % 7) - 3; } while (curr_vel.x == 0);
	do { curr_vel.y = (rand() % 7) - 3; } while (curr_vel.y == 0);

	lastProductionTime = pGame->getGameTime();
	canEat = true;
}

bool Animal::checkProduction() {
	unsigned long currentTime = pGame->getGameTime();
	if (currentTime - lastProductionTime >= (unsigned long)productionRate) {
		lastProductionTime = currentTime;
		return true;
	}
	return false;
}

void Animal::draw() const {
	window* pW = pGame->getWind();
	pW->DrawImage(image_path, curr_pos.x, curr_pos.y, width, height);

	int timeLeft = productionRate - (int)(pGame->getGameTime() - lastProductionTime);
	if (timeLeft < 0) timeLeft = 0;

	pW->SetPen(BLACK);
	pW->DrawString(curr_pos.x + 5, curr_pos.y - 5, std::to_string(timeLeft) + "s");
}

bool Animal::isColliding(const Animal* other) const {
	if (!other) return false;

	return (curr_pos.x < other->curr_pos.x + other->width &&
		curr_pos.x + width > other->curr_pos.x &&
		curr_pos.y < other->curr_pos.y + other->height &&
		curr_pos.y + height > other->curr_pos.y);
}


Chick::Chick(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path) {
	productionRate = 10;
}

void Chick::moveStep() {
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;

	if (RefPoint.x <= 0) {
		RefPoint.x = 0;
		curr_vel.x = -curr_vel.x;
	}
	else if (RefPoint.x + width >= config.windWidth) {
		RefPoint.x = config.windWidth - width;
		curr_vel.x = -curr_vel.x;
	}

	int topLimit = 2 * config.toolBarHeight;
	int bottomLimit = config.windHeight - config.statusBarHeight;

	if (RefPoint.y <= topLimit) {
		RefPoint.y = topLimit;
		curr_vel.y = -curr_vel.y;
	}
	else if (RefPoint.y + height >= bottomLimit) {
		RefPoint.y = bottomLimit - height;
		curr_vel.y = -curr_vel.y;
	}

	if (RefPoint.x < 300 && RefPoint.y + height > bottomLimit - 300) {
		RefPoint.x -= curr_vel.x;
		RefPoint.y -= curr_vel.y;
		curr_vel.x = -curr_vel.x;
		curr_vel.y = -curr_vel.y;
	}

	curr_pos = RefPoint;
}


Cow::Cow(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path) {
	productionRate = 10;
}

void Cow::moveStep() {
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;

	if (RefPoint.x <= 0) {
		RefPoint.x = 0;
		curr_vel.x = -curr_vel.x;
	}
	else if (RefPoint.x + width >= config.windWidth) {
		RefPoint.x = config.windWidth - width;
		curr_vel.x = -curr_vel.x;
	}

	int topLimit = 2 * config.toolBarHeight;
	int bottomLimit = config.windHeight - config.statusBarHeight;

	if (RefPoint.y <= topLimit) {
		RefPoint.y = topLimit;
		curr_vel.y = -curr_vel.y;
	}
	else if (RefPoint.y + height >= bottomLimit) {
		RefPoint.y = bottomLimit - height;
		curr_vel.y = -curr_vel.y;
	}

	if (RefPoint.x < 300 && RefPoint.y + height > bottomLimit - 300) {
		RefPoint.x -= curr_vel.x;
		RefPoint.y -= curr_vel.y;
		curr_vel.x = -curr_vel.x;
		curr_vel.y = -curr_vel.y;
	}

	curr_pos = RefPoint;
}


Seal::Seal(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path) {
	productionRate = 10;
}

void Seal::moveStep() {
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;

	int topLimit = config.windHeight - config.statusBarHeight - 300;
	int bottomLimit = config.windHeight - config.statusBarHeight;
	int rightLimit = 300;

	if (RefPoint.x <= 0) {
		RefPoint.x = 0;
		curr_vel.x = -curr_vel.x;
	}
	else if (RefPoint.x + width >= rightLimit) {
		RefPoint.x = rightLimit - width;
		curr_vel.x = -curr_vel.x;
	}

	if (RefPoint.y <= topLimit) {
		RefPoint.y = topLimit;
		curr_vel.y = -curr_vel.y;
	}
	else if (RefPoint.y + height >= bottomLimit) {
		RefPoint.y = bottomLimit - height;
		curr_vel.y = -curr_vel.y;
	}

	curr_pos = RefPoint;
}

Dog::Dog(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path) {
	creationTime = pGame->getGameTime();
	lifeSpan = 20;
	productionRate = lifeSpan;
}

void Dog::draw() const {
	static image dogSprite("images\\dog_sprite.jpg");
	window* pW = pGame->getWind();
	pW->DrawImage(dogSprite, curr_pos.x, curr_pos.y, width, height);

	int timeLeft = lifeSpan - (int)(pGame->getGameTime() - creationTime);
	if (timeLeft < 0) timeLeft = 0;

	pW->SetPen(BLACK);
	pW->DrawString(curr_pos.x + 5, curr_pos.y - 5, std::to_string(timeLeft) + "s");
}

void Dog::moveStep() {
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;

	if (RefPoint.x <= 0) {
		RefPoint.x = 0;
		curr_vel.x = -curr_vel.x;
	}
	else if (RefPoint.x + width >= config.windWidth) {
		RefPoint.x = config.windWidth - width;
		curr_vel.x = -curr_vel.x;
	}

	int topLimit = 2 * config.toolBarHeight;
	int bottomLimit = config.windHeight - config.statusBarHeight;

	if (RefPoint.y <= topLimit) {
		RefPoint.y = topLimit;
		curr_vel.y = -curr_vel.y;
	}
	else if (RefPoint.y + height >= bottomLimit) {
		RefPoint.y = bottomLimit - height;
		curr_vel.y = -curr_vel.y;
	}

	if (RefPoint.x < 300 && RefPoint.y + height > bottomLimit - 300) {
		RefPoint.x -= curr_vel.x;
		RefPoint.y -= curr_vel.y;
		curr_vel.x = -curr_vel.x;
		curr_vel.y = -curr_vel.y;
	}

	curr_pos = RefPoint;
}

bool Dog::isExpired() const {
	return (pGame->getGameTime() - creationTime >= (unsigned long)lifeSpan);
}


Wolf::Wolf(Game* r_pGame, point r_point, int r_width, int r_height, int r_speed)
	: Animal(r_pGame, r_point, r_width, r_height, "images\\wolf.jpg"), speed(r_speed), clickCount(0) {

	WolfState state;
	state.exactX = r_point.x;
	state.exactY = r_point.y;
	state.angle = (rand() % 360) * 3.14159 / 180.0;
	state.turnDir = (rand() % 2 == 0) ? 1 : -1;

	wolfStates[this] = state;
}

Wolf::~Wolf() {
	wolfStates.erase(this);
}

void Wolf::setSpeed(int newSpeed) {
	speed = newSpeed;
}

bool Wolf::isClicked(int x, int y) const {
	return x >= curr_pos.x && x <= curr_pos.x + width &&
		y >= curr_pos.y && y <= curr_pos.y + height;
}

int Wolf::incrementClickCount() {
	return ++clickCount;
}

void Wolf::draw() const {
	pGame->getWind()->DrawImage(image_path, curr_pos.x, curr_pos.y, width, height);
}

void Wolf::moveStep() {
	curr_pos.x += curr_vel.x;
	curr_pos.y += curr_vel.y;
	WolfState& state = wolfStates[this];

	if (rand() % 40 == 0) {
		state.turnDir = -state.turnDir;
	}

	state.angle += state.turnDir * 0.08;

	double moveSpeed = (speed < 2) ? 2.5 : (double)speed;
	state.exactX += moveSpeed * cos(state.angle);
	state.exactY += moveSpeed * sin(state.angle);

	bool bounced = false;
	if (state.exactX <= 0) {
		state.exactX = 0;
		state.angle = 3.14159 - state.angle;
		bounced = true;
	}
	else if (state.exactX + width >= config.windWidth) {
		state.exactX = config.windWidth - width;
		state.angle = 3.14159 - state.angle;
		bounced = true;
	}

	int topLimit = 2 * config.toolBarHeight;
	int bottomLimit = config.windHeight - config.statusBarHeight;

	if (state.exactY <= topLimit) {
		state.exactY = topLimit;
		state.angle = -state.angle;
		bounced = true;
	}
	else if (state.exactY + height >= bottomLimit) {
		state.exactY = bottomLimit - height;
		state.angle = -state.angle;
		bounced = true;
	}

	if (state.exactX < 300 && state.exactY + height > bottomLimit - 300) {
		state.exactX -= moveSpeed * cos(state.angle);
		state.exactY -= moveSpeed * sin(state.angle);
		state.angle = state.angle + 3.14159;
		bounced = true;
	}

	if (bounced) {
		state.turnDir = (rand() % 2 == 0) ? 1 : -1;
	}

	RefPoint.x = (int)std::round(state.exactX);
	RefPoint.y = (int)std::round(state.exactY);
	curr_pos = RefPoint;
}


Cat::Cat(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: GameObject(r_pGame, r_point, r_width, r_height, GREEN, BLACK) {
	image_path = img_path;
	curr_pos = r_point;
	collectRadius = 60;

	do { curr_vel.x = (rand() % 5) - 2; } while (curr_vel.x == 0);
	do { curr_vel.y = (rand() % 5) - 2; } while (curr_vel.y == 0);
}

void Cat::draw() const {
	window* pW = pGame->getWind();

	static image catSprite("images\\cat.jpg");

	pW->DrawImage(catSprite, curr_pos.x, curr_pos.y, width, height);

	pW->SetPen(WHITE);
	pW->SetFont(14, BOLD, BY_NAME, "Arial");
	pW->DrawString(curr_pos.x + 5, curr_pos.y - 5, "CAT");
}

void Cat::moveStep() {
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;

	if (RefPoint.x <= 0) {
		RefPoint.x = 0;
		curr_vel.x = -curr_vel.x;
	}
	else if (RefPoint.x + width >= config.windWidth) {
		RefPoint.x = config.windWidth - width;
		curr_vel.x = -curr_vel.x;
	}

	int topLimit = 2 * config.toolBarHeight;
	int bottomLimit = config.windHeight - config.statusBarHeight;

	if (RefPoint.y <= topLimit) {
		RefPoint.y = topLimit;
		curr_vel.y = -curr_vel.y;
	}
	else if (RefPoint.y + height >= bottomLimit) {
		RefPoint.y = bottomLimit - height;
		curr_vel.y = -curr_vel.y;
	}

	if (RefPoint.x < 300 && RefPoint.y + height > bottomLimit - 300) {
		RefPoint.x -= curr_vel.x;
		RefPoint.y -= curr_vel.y;
		curr_vel.x = -curr_vel.x;
		curr_vel.y = -curr_vel.y;
	}

	curr_pos = RefPoint;
}


Grass::Grass(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: GameObject(r_pGame, r_point, r_width, r_height, GREEN, BLACK) {
	image_path = img_path;
	creationTime = pGame->getGameTime();
	lifeSpan = 10;
}

void Grass::draw() const {
	pGame->getWind()->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);

	int timeLeft = lifeSpan - (int)(pGame->getGameTime() - creationTime);
	if (timeLeft < 0) timeLeft = 0;

	pGame->getWind()->SetPen(BLACK);
	pGame->getWind()->SetFont(16, BOLD, BY_NAME, "Arial");
	pGame->getWind()->DrawString(RefPoint.x + 5, RefPoint.y - 5, std::to_string(timeLeft) + "s");
}

void Grass::moveStep() {}

bool Grass::isExpired() const {
	return (pGame->getGameTime() - creationTime >= (unsigned long)lifeSpan);
}