#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../CMUgraphicsLib/auxil.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../Entities/Animal.h"
#include "Warehouse.h" 
#include "../Core/GameObject.h"
#include "AudioManager.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>

class audioManager;

class Product;
class FoodArea;
class Animal;

class Game
{
private:
	string playerName;
	std::vector<FoodArea*> foodList;
	std::vector<Animal*> animalList;
	std::vector<Product*> productList;
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	Warehouse* pWarehouse;
	AudioManager* audioManager;

	std::vector<Chick*> chicks;
	std::vector<Wolf*> wolves;
	int currentLevel;
	unsigned long gameStartTime;
	unsigned long lastWolfSpawnTime;
	unsigned long currentGameTime = 0;
	bool isPaused = false;
	int GetLevelGoal(int levelNumber) const;
	int GetLevelTimeLimit(int levelNumber) const;
	unsigned long GetWolfSpawnInterval() const;
	int GetWolfCountPerSpawn() const;
	int GetWolfSpeed() const;
	void updateWolfDifficulty(int currentLevel);
	void spawnWolfEveryInterval();
	void ApplyLevelUp();

	void promptUsername();
	void updateAndDisplayLeaderboard();


public:
	unsigned long getGameTime() const { return currentGameTime; }
	int budget = 6000;
	int remainingTimeSeconds = 120;
	int animalCount = 0;
	int grassCount = 0;
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
	void saveGame();
	void loadGame();
	void drawField() const;
	void drawStatusBar() const;
	void showWarehouse();
	bool SellWarehouseProduct(ProductType productType, int quantityToSell);
	bool checkLevelProgress(int currentBudget, int currentLevel);

	void go();

	void setPaused(bool pause);
	bool isGamePaused() const;

	window* getWind() const;
	AudioManager* getAudioManager() const { return audioManager; }

	void addProduct(Product* p);
	const std::vector<Wolf*>& getWolves() const { return wolves; }
	void removeWolf(const Wolf* wolf);
};
