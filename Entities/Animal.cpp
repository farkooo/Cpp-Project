#include "Animal.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
#include <cstdlib>
using namespace std;

Animal::Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
	curr_pos = r_point;
	curr_vel.x = (rand() % 7) - 3;
	curr_vel.y = (rand() % 7) - 3;

}

void Animal::draw() const
{
	//draw image of this object
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, curr_pos.x, curr_pos.y, width, height);
}

void Animal::clearPreviousPosition() const
{
	window* pWind = pGame->getWind();
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(prev_pos.x, prev_pos.y, prev_pos.x + width, prev_pos.y + height);
}

Chick::Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path)
{}

void Chick::moveStep()
{
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;

	curr_pos = RefPoint;

	if (RefPoint.x <= 0 || (RefPoint.x + width >= config.windWidth)) {
		curr_vel.x = -curr_vel.x;
		RefPoint.x += curr_vel.x;
	}

	if (RefPoint.y <= 2 * config.toolBarHeight || (RefPoint.y + height) >= (config.windHeight - config.statusBarHeight)) {
		curr_vel.y = -curr_vel.y;
		RefPoint.y += curr_vel.y;
	}
	//TO DO: add code for cleanup and game exit here
	/*
	//draw image of this object in the field
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
	*/
	
}

Cow::Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path)
{}

void Cow::moveStep()
{

	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;

	curr_pos = RefPoint;

	if (RefPoint.x <= 0 || (RefPoint.x + width >= config.windWidth)) {
		curr_vel.x = -curr_vel.x;
		RefPoint.x += curr_vel.x;
	}

	if (RefPoint.y <= 2 * config.toolBarHeight || (RefPoint.y + height) >= (config.windHeight - config.statusBarHeight)) {
		curr_vel.y = -curr_vel.y;
		RefPoint.y += curr_vel.y;
	}
	//TO DO: add code for cleanup and game exit here


}

Wolf::Wolf(Game* r_pGame, point r_point, int r_width, int r_height, int r_speed)
	: Animal(r_pGame, r_point, r_width, r_height, "images\\wolf.jpg"), speed(r_speed)
{
	curr_vel.x = r_speed;
	curr_vel.y = 0;
}

void Wolf::draw() const
{
	Animal::draw();
}

void Wolf::moveStep()
{
	prev_pos = curr_pos;

	static bool isSeeded = false;
	if (!isSeeded)
	{
		srand(static_cast<unsigned int>(CurrentTime()));
		isSeeded = true;
	}

	curr_vel.x += (rand() % 3) - 1;
	curr_vel.y += (rand() % 3) - 1;

	if (curr_vel.x > speed)
		curr_vel.x = speed;
	else if (curr_vel.x < -speed)
		curr_vel.x = -speed;

	if (curr_vel.y > speed)
		curr_vel.y = speed;
	else if (curr_vel.y < -speed)
		curr_vel.y = -speed;

	if (curr_vel.x == 0 && curr_vel.y == 0)
		curr_vel.x = (rand() % 2 == 0) ? 1 : -1;

	curr_pos.x += curr_vel.x;
	curr_pos.y += curr_vel.y;

	const int minX = 0;
	const int maxX = config.windWidth - width;
	const int minY = 2 * config.toolBarHeight;
	const int maxY = config.windHeight - config.statusBarHeight - height;

	if (curr_pos.x < minX)
	{
		curr_pos.x = minX;
		curr_vel.x = -curr_vel.x;
	}
	else if (curr_pos.x > maxX)
	{
		curr_pos.x = maxX;
		curr_vel.x = -curr_vel.x;
	}

	if (curr_pos.y < minY)
	{
		curr_pos.y = minY;
		curr_vel.y = -curr_vel.y;
	}
	else if (curr_pos.y > maxY)
	{
		curr_pos.y = maxY;
		curr_vel.y = -curr_vel.y;
	}
Seal::Seal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path)
{}
	
void Seal::moveStep()
{
	RefPoint.x += curr_vel.x;
	RefPoint.y += curr_vel.y;

	curr_pos = RefPoint;

	if (RefPoint.x <= 0 || (RefPoint.x + width >= config.windWidth)) {
		curr_vel.x = -curr_vel.x;
		RefPoint.x += curr_vel.x;
	}

	if (RefPoint.y <= 2 * config.toolBarHeight || (RefPoint.y + height) >= (config.windHeight - config.statusBarHeight)) {
		curr_vel.y = -curr_vel.y;
		RefPoint.y += curr_vel.y;
	}
	//TO DO: add code for cleanup and game exit here

}

Grass::Grass(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
	curr_pos = r_point;

}

void Grass::draw() const {
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}
void Grass::moveStep()
{
	//TO DO: add code for cleanup and game exit here
	

}
