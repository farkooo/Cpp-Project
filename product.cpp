#include "Product.h"
#include "../Core/Game.h"

// --- Constructor الأب ---
Product::Product(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
}

// --- دالة الرسم المشتركة (Task Implementation) ---
void Product::draw() const
{
	window* pWind = pGame->getWind();
	// بنرسم الصورة بناءً على المسار والإحداثيات الموروثة
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

// --- Check if the item is clicked (Task Implementation) ---
bool Product::isClicked(int x, int y) const
{
	return (x >= RefPoint.x && x <= RefPoint.x + width && 
			y >= RefPoint.y && y <= RefPoint.y + height);
}

// --- Constructor البيضة ---
Egg::Egg(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Product(r_pGame, r_point, r_width, r_height, img_path)
{
}

// --- Constructor اللبن ---
Milk::Milk(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Product(r_pGame, r_point, r_width, r_height, img_path)
{
}

// --- Constructor السمكة ---
Fish::Fish(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Product(r_pGame, r_point, r_width, r_height, img_path)
{
}