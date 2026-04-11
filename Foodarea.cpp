#include "FoodArea.h"
#include "../Core/Game.h"
#include <string> // عشان نستخدم to_string

FoodArea::FoodArea(Game* r_pGame, point r_point, int r_width, int r_height, color fc, color bc, int initialAmount)
    : GameObject(r_pGame, r_point, r_width, r_height, fc, bc)
{
    foodCount = initialAmount;

    // 🌟 تسجيل لحظة الزراعة وعمر العشب
    creationTime = time(0);
    lifeSpan = 10; // 10 ثواني ويختفي
}

void FoodArea::consume() {
    if (foodCount > 0) foodCount--;
}

bool FoodArea::isEmpty() const {
    // احسب الوقت اللي فات
    double secondsPassed = difftime(time(0), creationTime);

    // لازم ترجع true لو الوقت عدا الـ lifespan أو الأكل خلص
    return (secondsPassed >= lifeSpan || foodCount <= 0);
}

void FoodArea::draw() const {
    if (!isEmpty()) {
        window* pW = pGame->getWind();

        // 1. رسم المربع الأخضر
        pW->SetPen(borderColor, 3);
        pW->SetBrush(fillColor);
        pW->DrawRectangle(RefPoint.x, RefPoint.y, RefPoint.x + width, RefPoint.y + height);

        // --- 🌟 رسم العداد التنازلي فوق العشب ---
        int timeAlive = difftime(time(0), creationTime); // كام ثانية عاشها؟
        int timeLeft = lifeSpan - timeAlive;             // فاضل كام ثانية؟
        if (timeLeft < 0) timeLeft = 0; // عشان ميكتبش رقم بالسالب

        // إعدادات الخط للعداد
        pW->SetPen(BLACK, 50); // لون الخط أسود
        pW->SetFont(20, BOLD, BY_NAME, "Arial"); // حجم الخط 20

        // رسم الرقم في منتصف المربع تقريباً (بنجمع 15 على إحداثيات المربع)
        pW->DrawString(RefPoint.x + 15, RefPoint.y + 15, to_string(timeLeft) + "s");
    }
}