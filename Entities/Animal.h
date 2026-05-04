#pragma once
#include "../Core/Drawable.h"
#include "../Core/GameObject.h"
#include <string>

class Animal : public GameObject
{
protected:
	std::string image_path;
	point curr_pos;
	point prev_pos;
	point curr_vel;

	unsigned long lastProductionTime;
	int productionRate;
	bool canEat;
public:
	Animal(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);

	virtual void draw() const override;
	virtual void moveStep() = 0;
	point getPos() const { return curr_pos; }

	bool checkProduction();
	bool isColliding(const Animal* other) const;
	bool getCanEat() const { return canEat; }
	void setCanEat(bool b) { canEat = b; }
};

class Chick : public Animal
{
public:
	Chick(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
	virtual void moveStep() override;
};

class Cow : public Animal
{
public:
	Cow(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
	virtual void moveStep() override;
};

class Seal : public Animal
{
public:
	Seal(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
	virtual void moveStep() override;
};

class Dog : public Animal
{
private:
	unsigned long creationTime;
	int lifeSpan;
public:
	Dog(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
	virtual void draw() const override;
	virtual void moveStep() override;
	bool isExpired() const;
};

class Wolf : public Animal
{
private:
	int speed;
	int clickCount;
public:
	Wolf(Game* r_pGame, point r_point, int r_width, int r_height, int r_speed = 1);
	~Wolf();
	virtual void draw() const override;
	virtual void moveStep() override;
	void setSpeed(int newSpeed);
	bool isClicked(int x, int y) const;
	int incrementClickCount();
	int getClickCount() const { return clickCount; }
};

class Grass : public GameObject
{
private:
	std::string image_path;
	unsigned long creationTime;
	int lifeSpan;
public:
	point curr_pos;
	Grass(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
	virtual void draw() const override;

	virtual void moveStep();

	bool isExpired() const;
};
