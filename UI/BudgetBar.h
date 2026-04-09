#pragma once
#include "../Core/Drawable.h"
#include "../Entities/Animal.h"
#include "../Config/GameConfig.h"
#include <random>
#include <limits>

//Ranges for generating a random location for the animals
const int range_min_x = 50;
const int range_max_x = config.windWidth - 50;
const int range_min_y = (config.toolBarHeight * 2) + 50;
const int range_max_y = config.windHeight - config.statusBarHeight - 50;

const int MAX_CREATED_ANIMALS = 15;


//Base class for all toolbar icons 
class BudgetbarIcon :public Drawable
{
private:
	//string image_path;
public:
	string image_path;
	BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick() = 0;   //The action that should be taken when this icon is clicked
	virtual void update() {}
	virtual void reset() = 0;
};

class ChickIcon : public BudgetbarIcon
{
public:
	Chick** chickList; //an array of Chick pointers
	int count = 0;
	ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
	void update();
	virtual void reset() override;
};

class CowIcon : public BudgetbarIcon
{
public:
	Cow** cowList; //an array of cow pointers
	int count = 0;
	CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
	void update();
	virtual void reset() override;

};

class SealIcon : public BudgetbarIcon
{
public:
	Seal** sealList; //an array of cow pointers
	int count = 0;
	SealIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
	void update();
	virtual void reset() override;

};

class WaterIcon : public BudgetbarIcon
{
public:
	Grass** grassList; //an array of grass pointers
	int count = 0;
	WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
	void update();
	virtual void reset() override;

};



// TO DO: The rest of icons in the toolbar

enum ANIMAL_ICONS //The icons of the toolbar (you should add more icons)
{
	//Note: Icons are ordered here as they appear in menu
	//If you want to change the menu icons order, change the order here
	ICON_CHICK,

	//TODO: Add more icons names here
	ICON_COW,
	//Cow icon
	ICON_SEAL,
	ICON_WATER,
	ANIMAL_COUNT		//no. of menu icons ==> This should be the last line in this enum
};

class Budgetbar : public Drawable
{
private:
	BudgetbarIcon** iconsList; //an array of toolbarIcon pointers
	string iconsImages[ANIMAL_COUNT];

public:
	Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height);
	~Budgetbar();
	void draw() const override;
	bool handleClick(int x, int y);	//handles clicks on toolbar icons, returns true if exit is clicked
	void update();
	void reset();

};
