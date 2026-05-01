#include "GameObject.h"

GameObject::GameObject(Game* r_pGame, point ref, int r_width, int r_height, color fc, color bc) : Drawable (r_pGame, ref, r_width, r_height)
{
	fillColor = fc;
	borderColor = bc;
}

void GameObject::setRefPoint(point p)
{
	RefPoint = p;
}


// you should implement this function knowing that each game object has refPoint, width and height 
// for simplicity, you can consider all game objects are rectangles
bool GameObject::CollisionDetection(const GameObject& gObj)
{
	// All game objects are treated as rectangles
	int x1 = RefPoint.x;
	int y1 = RefPoint.y;
	int w1 = width;
	int h1 = height;

	int x2 = gObj.RefPoint.x;
	int y2 = gObj.RefPoint.y;
	int w2 = gObj.width;
	int h2 = gObj.height;

	// Check for overlap
	if (x1 < x2 + w2 && x1 + w1 > x2 &&
		y1 < y2 + h2 && y1 + h1 > y2)
	{
		return true;
	}

	return false;
}
