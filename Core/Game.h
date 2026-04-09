#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../CMUgraphicsLib/auxil.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../Entities/Animal.h"
#include <vector>

class Game
{
private:
	window* pWind;	//Pointer to the CMU graphics window
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	std::vector<Chick*> chicks;
	std::vector<Wolf*> wolves;
	int currentLevel;
	unsigned long gameStartTime;
	unsigned long lastWolfSpawnTime;

public:
	int budget = 2000;
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
	void addChick(point position, int width, int height, const string& imagePath);
	void redrawChicks() const;
	void drawWolf(point position, int width, int height, int speed = 1);
	void generateRandomWolves();
	int getCurrentLevel() const;
	void restartGame();

	void go();

	window* getWind() const;		//returns a pointer to the graphics window
};

