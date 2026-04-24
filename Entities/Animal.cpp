#include "Animal.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
#include <ctime>
#include <cmath>
#include <unordered_map>

// --- Helper map to track precise circular motion for wolves ---
struct WolfState {
	double exactX;
	double exactY;
	double angle;
	int turnDir;
};
// This allows us to give the wolves advanced movement without altering Animal.h
static std::unordered_map<const Wolf*, WolfState> wolfStates;


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

	// Initialize circular motion parameters
	WolfState state;
	state.exactX = r_point.x;
	state.exactY = r_point.y;
	state.angle = (rand() % 360) * 3.14159 / 180.0; // Random starting angle
	state.turnDir = (rand() % 2 == 0) ? 1 : -1;     // Left or right circles

	wolfStates[this] = state;
}

void Wolf::draw() const { Animal::draw(); }

void Wolf::moveStep() {
	// Grab the custom circular state for THIS specific wolf
	WolfState& state = wolfStates[this];

	// Randomly change circle direction occasionally (makes it loop like an '8' or wander erratically)
	if (rand() % 40 == 0) {
		state.turnDir = -state.turnDir;
	}

	// Update the angle to make the path curve (0.08 radians per frame creates nice wide circles)
	state.angle += state.turnDir * 0.08;

	// Because speed can be low, we calculate in floating point so the circle remains smooth
	double moveSpeed = (speed < 2) ? 2.5 : (double)speed; // Give it a base prowl speed
	state.exactX += moveSpeed * cos(state.angle);
	state.exactY += moveSpeed * sin(state.angle);

	// Boundary bouncing logic using angle reflection
	bool bounced = false;
	if (state.exactX <= 0) {
		state.exactX = 0;
		state.angle = 3.14159 - state.angle; // Reflect horizontally
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
		state.angle = -state.angle; // Reflect vertically
		bounced = true;
	}
	else if (state.exactY + height >= bottomLimit) {
		state.exactY = bottomLimit - height;
		state.angle = -state.angle;
		bounced = true;
	}

	// If it hit a wall, force a direction change so it doesn't scrape along the edge forever
	if (bounced) {
		state.turnDir = (rand() % 2 == 0) ? 1 : -1;
	}

	// Update base class integer coordinates so the drawing function works perfectly
	RefPoint.x = (int)std::round(state.exactX);
	RefPoint.y = (int)std::round(state.exactY);
	curr_pos = RefPoint;
}

// --- Grass ---
Grass::Grass(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
	: Drawable(r_pGame, r_point, r_width, r_height) {
	image_path = img_path;
}

void Grass::draw() const { pGame->getWind()->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height); }

void Grass::moveStep() {}