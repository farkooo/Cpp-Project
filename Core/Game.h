#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"

class Game
{
private:
	window* pWind;	//Pointer to the CMU graphics window
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;

public:
	int budget = 6000;
	int remainingTimeSeconds = 120; // Current level timer (modify as needed)
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
	void drawTimer() const; // Function to draw the timer HUD
	void clearStatusBar() const;	//Clears the status bar


	void printMessage(string msg) const;	//Print a message on Status bar
	void drawField() const;
	void drawStatusBar() const;

	void go();

	window* getWind() const;		//returns a pointer to the graphics window
};

