#include "FoodArea.h"
#include "../Core/Game.h"

FoodArea::FoodArea(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path, int initialAmount)
    : GameObject(r_pGame, r_point, r_width, r_height, GREEN, BLACK) // Keeps base colors just in case
{
    image_path = img_path;
    foodCount = initialAmount;
    creationTime = pGame->getGameTime();
    lifeSpan = 10;
}

void FoodArea::consume() {
    if (foodCount > 0) foodCount--;
}

bool FoodArea::isEmpty() const {
    return (pGame->getGameTime() - creationTime >= (unsigned long)lifeSpan || foodCount <= 0);
}

void FoodArea::draw() const {
    window* pW = pGame->getWind();

    
    pW->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);

    
    int timeLeft = lifeSpan - (int)(pGame->getGameTime() - creationTime);
    if (timeLeft < 0) timeLeft = 0;

    
    pW->SetPen(BLACK); 
    pW->SetFont(16, BOLD, BY_NAME, "Arial");
    pW->DrawString(RefPoint.x + 5, RefPoint.y - 5, std::to_string(timeLeft) + "s");
}