#include "Animal.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
#include <ctime> // 🌟 ضروري جداً عشان الـ time(0)

using namespace std;

Animal::Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
	curr_pos = r_point;
	curr_vel.x = 1;
	curr_vel.y = 1;

	// 🌟 تصفير عداد الإنتاج لحظة إنشاء الحيوان
	lastProductionTime = time(0);
}

// 🌟 دالة فحص الوقت (Logic Task 19)
bool Animal::checkProduction()
{
	time_t currentTime = time(0);
	// طرح الوقت الحالي من وقت آخر إنتاج ومقارنته بالمعدل (10 أو 15)
	if (currentTime - lastProductionTime >= productionRate)
	{
		lastProductionTime = currentTime; // إعادة ضبط العداد للدورة القادمة
		return true;
	}
	return false;
}

void Animal::draw() const
{
	//draw image of this object
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

Chick::Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path)
{
	// 🌟 تحديد معدل الفرخة (10 ثواني)
	productionRate = 10;
}

void Chick::moveStep()
{
	//TO DO: add code for cleanup and game exit here
	/*	//draw image of this object in the field	window* pWind = pGame->getWind();	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);	*/
	cout << "Icon Chick Clicked" << endl;
}

Cow::Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Animal(r_pGame, r_point, r_width, r_height, img_path)
{
	// 🌟 تحديد معدل البقرة (15 ثانية)
	productionRate = 15;
}

void Cow::moveStep()
{
	//TO DO: add code for cleanup and game exit here
	cout << "Icon Cow Clicked" << endl;
}