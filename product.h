#pragma once
#include "../Core/Drawable.h"
#include <string>
#include "../Warehouse.h" 

class Product : public Drawable
{
protected:
	string image_path;
public:
	Product(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);

	virtual void draw() const override;
	virtual ProductType getType() const = 0;

	bool isClicked(int x, int y) const;
	point getPos() const { return RefPoint; }
};

class Egg : public Product
{
public:
	Egg(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual ProductType getType() const override { return ProductType::EGG; }
};

class Milk : public Product
{
public:
	Milk(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual ProductType getType() const override { return ProductType::MILK; }
};

class Fish : public Product
{
public:
	Fish(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual ProductType getType() const override { return ProductType::FISH; }
};