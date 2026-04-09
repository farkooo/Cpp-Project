#include "Game.h"
#include "../Config/GameConfig.h"
#include <random>

Game::Game()
{
	budget = 2000;
	currentLevel = 1;
	gameStartTime = CurrentTime();
	lastWolfSpawnTime = gameStartTime;

	//1 - Create the main window
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);
	pWind->SetBuffering(true);
	pWind->SetBrush(config.bkGrndColor);
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->DrawRectangle(0, 0, config.windWidth, config.windHeight);

	//2 - create and draw the toolbar
	createToolbar();
	createBudgetbar();
	//3 - create and draw the backgroundPlayingArea


	//4- Create the Plane
	//TODO: Add code to create and draw the Plane

	//5- Create the Bullet
	//TODO: Add code to create and draw the Bullet

	//7- Create and clear the status bar
	clearStatusBar();
	printBudget("BUDGET = $" + to_string(budget));
	pWind->UpdateBuffer();
}

Game::~Game()
{
	for (size_t i = 0; i < chicks.size(); i++)
		delete chicks[i];
	for (size_t i = 0; i < wolves.size(); i++)
		delete wolves[i];
	delete gameToolbar;
	delete gameBudgetbar;
	delete pWind;
}

clicktype Game::getMouseClick(int& x, int& y) const
{
	return pWind->WaitMouseClick(x, y);	//Wait for mouse click

}

string Game::getSrting() const
{
	string Label;
	char Key;
	keytype ktype;
	pWind->FlushKeyQueue();
	while (1)
	{
		ktype = pWind->WaitKeyPress(Key);
		if (ktype == ESCAPE)	//ESCAPE key is pressed
			return "";	//returns nothing as user has cancelled label
		if (Key == 13)	//ENTER key is pressed
			return Label;
		if (Key == 8)	//BackSpace is pressed
			if (Label.size() > 0)
				Label.resize(Label.size() - 1);
			else
				Key = '\0';
		else
			Label += Key;
		printMessage(Label);
	}
}

window* Game::CreateWind(int w, int h, int x, int y) const
{
	window* pW = new window(w, h, x, y);
	pW->SetBrush(config.bkGrndColor);
	pW->SetPen(config.bkGrndColor, 1);
	pW->DrawRectangle(0, 0, w, h);
	return pW;
}

void Game::createToolbar() 
{
	point toolbarUpperleft;
	toolbarUpperleft.x = 0;
	toolbarUpperleft.y = 0;

	gameToolbar = new Toolbar(this, toolbarUpperleft, 0, config.toolBarHeight);
	gameToolbar->draw();
}

void Game::createBudgetbar()
{
	point budgetbarUpperleft;
	budgetbarUpperleft.x = 0;
	budgetbarUpperleft.y = config.toolBarHeight;

	gameBudgetbar = new Budgetbar(this, budgetbarUpperleft, config.windWidth, config.toolBarHeight);
	gameBudgetbar->draw();
}


void Game::clearBudget() const
{
	//Clear Status bar by drawing a filled rectangle
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(config.windWidth - 500, config.toolBarHeight, config.windWidth, 2*config.toolBarHeight);
}

void Game::printBudget(string msg) const
{
	clearBudget();	//First clear the status bar

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(config.windWidth-200, config.toolBarHeight + 10, msg);

}

void Game::clearStatusBar() const
{
	//Clear Status bar by drawing a filled rectangle
	pWind->SetPen(config.statusBarColor, 1);
	pWind->SetBrush(config.statusBarColor);
	pWind->DrawRectangle(0, config.windHeight - config.statusBarHeight, config.windWidth, config.windHeight);
}

void Game::clearPlayingArea() const
{
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth,
		config.windHeight - config.statusBarHeight);
}

void Game::printMessage(string msg) const
{
	clearStatusBar();	//First clear the status bar

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(10, config.windHeight - (int)(0.85 * config.statusBarHeight), msg);

}

void Game::addChick(point position, int width, int height, const string& imagePath)
{
	Chick* chick = new Chick(this, position, width, height, imagePath);
	chicks.push_back(chick);
	chick->draw();
}

void Game::redrawChicks() const
{
	for (size_t i = 0; i < chicks.size(); i++)
		chicks[i]->draw();
}

void Game::drawWolf(point position, int width, int height, int speed)
{
	Wolf* wolf = new Wolf(this, position, width, height, speed);
	wolves.push_back(wolf);
	wolf->draw();
}

void Game::generateRandomWolves()
{
	const unsigned long currentTime = CurrentTime();
	const unsigned long elapsedMs = currentTime - gameStartTime;
	currentLevel = static_cast<int>(elapsedMs / 30000UL) + 1;
	if (currentLevel < 1)
		currentLevel = 1;

	const int wolfWidth = 70;
	const int wolfHeight = 70;

	std::mt19937 generator(static_cast<unsigned int>(currentTime + currentLevel * 97));
	std::uniform_int_distribution<int> xDist(30, config.windWidth - wolfWidth - 30);
	std::uniform_int_distribution<int> yDist((2 * config.toolBarHeight) + 30,
		config.windHeight - config.statusBarHeight - wolfHeight - 30);

	point position;
	position.x = xDist(generator);
	position.y = yDist(generator);

	const int speed = currentLevel;
	drawWolf(position, wolfWidth, wolfHeight, speed);
}

int Game::getCurrentLevel() const
{
	return currentLevel;
}

void Game::restartGame()
{
	clearPlayingArea();

	for (size_t i = 0; i < wolves.size(); i++)
		delete wolves[i];
	wolves.clear();

	for (size_t i = 0; i < chicks.size(); i++)
		delete chicks[i];
	chicks.clear();

	budget = 2000;
	currentLevel = 1;
	gameStartTime = CurrentTime();
	lastWolfSpawnTime = gameStartTime;

	gameToolbar->draw();
	gameBudgetbar->draw();
	clearStatusBar();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage("Ready...");
	pWind->UpdateBuffer();
}

window* Game::getWind() const
{
	return pWind;
}

void Game::go()
{
	//This function reads the position where the user clicks to determine the desired operation
	int x, y;
	bool isExit = false;

	//Change the title
	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");

	do
	{
		const unsigned long currentTime = CurrentTime();
		if (currentTime - lastWolfSpawnTime >= 30000UL)
		{
			generateRandomWolves();
			lastWolfSpawnTime = currentTime;
		}

		printMessage("Ready...");
		string budget_string = "BUDGET = $" + to_string(budget);
		printBudget(budget_string);

		for (size_t i = 0; i < wolves.size(); i++)
			wolves[i]->clearPreviousPosition();

		redrawChicks();

		for (size_t i = 0; i < wolves.size(); i++)
		{
			wolves[i]->moveStep();
			wolves[i]->draw();
		}

		pWind->UpdateBuffer();
		x = -1;
		y = -1;
		clicktype click = pWind->GetMouseClick(x, y);

		if (click != NO_CLICK && y >= 0 && y < config.toolBarHeight)
		{
			isExit = gameToolbar->handleClick(x, y);
		}
		else if (click != NO_CLICK && y >= config.toolBarHeight && y < 2*config.toolBarHeight)
		{
			isExit = gameBudgetbar->handleClick(x, y);
		}

		Pause(50);

	} while (!isExit);
}


