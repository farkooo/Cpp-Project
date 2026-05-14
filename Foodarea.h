#pragma once
#include "../Core/GameObject.h"
#include <ctime>
#include <string>

class Game;

class FoodArea : public GameObject {
private:
    int foodCount;
    time_t creationTime;
    int lifeSpan;
    std::string image_path; 

public:
    FoodArea(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path, int initialAmount = 100);

    void consume();
    virtual void draw() const override;
    bool isEmpty() const;

    // Accessors used by the save system
    point getPos() const { return RefPoint; }
    int getFoodCount() const { return foodCount; }
};