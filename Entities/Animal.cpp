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
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
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
