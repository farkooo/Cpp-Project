#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../CMUgraphicsLib/auxil.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include <vector>
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
	window* pWind;	
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	std::vector<Chick*> chicks;
	std::vector<Wolf*> wolves;
	int currentLevel;
	unsigned long gameStartTime;
	unsigned long lastWolfSpawnTime;

public:
	int budget = 6000;
	int remainingTimeSeconds = 120; // Current level timer (modify as needed)
	int animalCount = 0;
	int level = 1;
	int goal = 5;
	time_t startTime;

	Game();
	~Game();

	clicktype getMouseClick(int& x, int& y) const; 
	string getSrting() const;	


	window* CreateWind(int, int, int, int) const; 
	void createToolbar();
	void createBudgetbar();
	void clearBudget() const;
	void printBudget(string msg) const;
	void drawTimer() const; // Function to draw the timer HUD
	void clearStatusBar() const;	
	void clearPlayingArea() const;


	void printMessage(string msg) const;	
	void drawWolf(point position, int width, int height, int speed = 1);
	void generateRandomWolves();
	int getCurrentLevel() const;
	void restartGame();
	void drawField() const;
	void drawStatusBar() const;
	void showWarehouse() const;

	void go();

	window* getWind() const;		
};

