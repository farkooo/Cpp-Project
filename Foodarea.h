#pragma once
#include "../Core/GameObject.h"
#include "../CMUgraphicsLib/auxil.h"
#include <ctime> // 🌟 ضروري عشان حساب الوقت

class Game;

class FoodArea : public GameObject {
private:
    int foodCount;
    time_t creationTime; // وقت زراعة العشب
    int lifeSpan;        // العمر الافتراضي بالثواني (10)

public:
    FoodArea(Game* r_pGame, point r_point, int r_width, int r_height, color fc, color bc, int initialAmount);

    void consume();
    virtual void draw() const override;
    bool isEmpty() const;
};