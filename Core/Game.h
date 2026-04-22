#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../CMUgraphicsLib/auxil.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../Entities/Animal.h"
#include "Warehouse.h" 
#include <vector>
#include <string>

class Product;
class FoodArea;
class Animal;

class Game
{
private:
	std::vector<FoodArea*> foodList;
	std::vector<Animal*> animalList;
	std::vector<Product*> productList;
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	Warehouse* pWarehouse;    

	std::vector<Chick*> chicks;
	std::vector<Wolf*> wolves;
	int currentLevel;
	unsigned long gameStartTime;
	unsigned long lastWolfSpawnTime;

public:
	int budget = 6000;
	int remainingTimeSeconds = 120;
	int animalCount = 0;
	int level = 1;
	int goal = 5;
	time_t startTime;

	Game();
	~Game();

	clicktype getMouseClick(int& x, int& y) const;
	std::string getSrting() const;

	window* CreateWind(int, int, int, int) const;
	void createToolbar();
	void createBudgetbar();
	void clearBudget() const;
	void printBudget(std::string msg) const;
	void drawTimer() const;
	void clearStatusBar() const;
	void clearPlayingArea() const;

	void printMessage(std::string msg) const;
	void drawWolf(point position, int width, int height, int speed = 1);
	void generateRandomWolves();
	int getCurrentLevel() const;
	void restartGame();
	void drawField() const;
	void drawStatusBar() const;
	void showWarehouse();

	void go();

	window* getWind() const;
};