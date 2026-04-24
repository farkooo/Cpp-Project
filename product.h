#pragma once
#include "../Core/Drawable.h"
#include <string>

class Product : public Drawable
{
protected:
	string image_path;
public:
	Product(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override; 
};

class Egg : public Product
{
public:
	Egg(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
};

class Milk : public Product
{
public:
	Milk(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
};
