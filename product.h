#pragma once
#include "../Core/Drawable.h"
#include <string>
#include "../Warehouse.h" // Include to get ProductType enum

class Product : public Drawable
{
protected:
	string image_path;
public:
	Product(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override; // دالة الرسم المشتركة

	// Returns the specific product type (like EGG or MILK) to pass to the warehouse
	virtual ProductType getType() const = 0;

	// Checks if a given coordinate is clicking on this product
	bool isClicked(int x, int y) const;
	virtual void draw() const override; 
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
