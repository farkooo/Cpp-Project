#include "Product.h"
#include "../Core/Game.h"

Product::Product(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
}

void Product::draw() const
{
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

bool Product::isClicked(int x, int y) const
{
	return (x >= RefPoint.x && x <= RefPoint.x + width &&
		y >= RefPoint.y && y <= RefPoint.y + height);
}

Egg::Egg(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Product(r_pGame, r_point, r_width, r_height, img_path)
{
}

Milk::Milk(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Product(r_pGame, r_point, r_width, r_height, img_path)
{
}

Fish::Fish(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Product(r_pGame, r_point, r_width, r_height, img_path)
{
}