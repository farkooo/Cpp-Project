#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../CMUgraphicsLib/auxil.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include <vector>
// 1. في أول الملف تحت الـ Forward Declarations
class Product;
class FoodArea;
class Animal;
#include "../Entities/Animal.h"
#include <vector>

class Game
{
private:
	vector<FoodArea*> foodList;
	vector<Animal*> animalList;
	vector<Product*> productList;
	window* pWind;	//Pointer to the CMU graphics window
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	std::vector<Chick*> chicks;
	std::vector<Wolf*> wolves;
	int currentLevel;
	unsigned long gameStartTime;
	unsigned long lastWolfSpawnTime;

public:
	int budget = 6000;
	int animalCount = 0;
	int level = 1;
	int goal = 5;
	time_t startTime;
	Game();
	~Game();

	clicktype getMouseClick(int& x, int& y) const; //Get coordinate where user clicks and returns click type (left/right)
	string getSrting() const;	 //Returns a string entered by the user


	window* CreateWind(int, int, int, int) const; //creates the game window
	void createToolbar();
	void createBudgetbar();
	void clearBudget() const;
	void printBudget(string msg) const;
	void clearStatusBar() const;	//Clears the status bar
	void clearPlayingArea() const;


	void printMessage(string msg) const;	//Print a message on Status bar
	void drawWolf(point position, int width, int height, int speed = 1);
	void generateRandomWolves();
	int getCurrentLevel() const;
	void restartGame();
	void drawField() const;
	void drawStatusBar() const;

	void go();

	window* getWind() const;		//returns a pointer to the graphics window
};

