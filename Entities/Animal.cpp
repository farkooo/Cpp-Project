#include "Animal.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
#include <ctime>

Animal::Animal(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
	curr_pos = r_point;
	curr_vel.x = (rand() % 7) - 3;
	curr_vel.y = (rand() % 7) - 3;
	lastProductionTime = pGame->getGameTime();
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
	pGame->getWind()->DrawImage(image_path, curr_pos.x, curr_pos.y, width, height);
}

// --- Chick ---
Chick::Chick(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path) {
	productionRate = 10;
}

void Chick::moveStep() {
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;
	curr_pos = RefPoint;
	if (RefPoint.x <= 0 || (RefPoint.x + width >= config.windWidth)) curr_vel.x = -curr_vel.x;
	if (RefPoint.y <= 2 * config.toolBarHeight || (RefPoint.y + height) >= (config.windHeight - config.statusBarHeight)) curr_vel.y = -curr_vel.y;
}

// --- Cow ---
Cow::Cow(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path) {
	productionRate = 15;
}

void Cow::moveStep() {
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;
	curr_pos = RefPoint;
	if (RefPoint.x <= 0 || (RefPoint.x + width >= config.windWidth)) curr_vel.x = -curr_vel.x;
	if (RefPoint.y <= 2 * config.toolBarHeight || (RefPoint.y + height) >= (config.windHeight - config.statusBarHeight)) curr_vel.y = -curr_vel.y;
}

// --- Seal ---
Seal::Seal(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path) {
}

void Seal::moveStep() {
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;
	curr_pos = RefPoint;
	if (RefPoint.x <= 0 || (RefPoint.x + width >= config.windWidth)) curr_vel.x = -curr_vel.x;
	if (RefPoint.y <= 2 * config.toolBarHeight || (RefPoint.y + height) >= (config.windHeight - config.statusBarHeight)) curr_vel.y = -curr_vel.y;
}

// --- Wolf ---
Wolf::Wolf(Game* r_pGame, point r_point, int r_width, int r_height, int r_speed)
	: Animal(r_pGame, r_point, r_width, r_height, "images\\wolf.jpg"), speed(r_speed) {
	curr_vel.x = r_speed;
	curr_vel.y = 0;
}

void Wolf::draw() const { Animal::draw(); }
void Wolf::moveStep() {
	curr_pos.x += curr_vel.x;
	curr_pos.y += curr_vel.y;
	// ... rest of wolf random logic
}

// --- Grass ---
Grass::Grass(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Drawable(r_pGame, r_point, r_width, r_height) {
	image_path = img_path;
}

void Grass::draw() const { pGame->getWind()->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height); }
void Grass::moveStep() {}