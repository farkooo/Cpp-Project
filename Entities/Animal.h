#pragma once

#include "../Core/Drawable.h"
#include <ctime> // 🌟 ضروري جداً عشان نوع البيانات time_t

class Animal : public Drawable
{
protected: // 🌟 خليناها protected عشان Chick و Cow يقدروا يغيروا الـ Rate
	string image_path;
	point curr_pos;
	point curr_vel;

	// --- متغيرات المهمة 19 (Production Logic) ---
	time_t lastProductionTime; // وقت آخر عملية إنتاج
	int productionRate;        // المدة المطلوبة (10 للفرخة و 15 للبقرة)

public:
	Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void moveStep() = 0;
	point getPos() const { return curr_pos; }

	// 🌟 الدالة اللي بتفحص هل جه وقت الإنتاج ولا لأ
	bool checkProduction();
};

class Chick : public Animal
{
public:
	Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep() override; // 🌟 ضفنا override للوضوح
};

class Cow : public Animal
{
public:
	Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep();
};

class Seal : public Animal
{
public:
	Seal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep();
};

class Grass : public Drawable
{
private:
	string image_path;
public:
	point curr_pos;
	Grass(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void moveStep();
};


