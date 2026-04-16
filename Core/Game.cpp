#include "Game.h"
#include "../Config/GameConfig.h"
#include <ctime>

Game::Game()
{
	//1 - Create the main window
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	//2 - create and draw the toolbar
	createToolbar();
	createBudgetbar();
	//3 - create and draw the backgroundPlayingArea


	//4- Create the Plane
	//TODO: Add code to create and draw the Plane

	//5- Create the Bullet
	//TODO: Add code to create and draw the Bullet

	//6- Create the enemies
	//TODO: Add code to create and draw enemies in random places

	//7- Create and clear the status bar
	clearStatusBar();
	startTime = time(NULL);
	drawField();
	drawWarehouse();
	drawStatusBar();
}

Game::~Game()
{
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

	gameBudgetbar = new Budgetbar(this, budgetbarUpperleft, 0, config.toolBarHeight);
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

void Game::printMessage(string msg) const
{
	clearStatusBar();	//First clear the status bar

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(10, config.windHeight - (int)(0.85 * config.statusBarHeight), msg);

}

void Game::drawField() const
{
	pWind->SetPen(BROWN, 10);
	pWind->SetBrush(GREEN);
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);
}

void Game::drawWarehouse() const
{
	int wh_x = config.windWidth - 200;
	int wh_y = config.windHeight - config.statusBarHeight - 140;
	int wh_w = 180;
	int wh_h = 100;

	pWind->SetBrush(BROWN);
	pWind->SetPen(BLACK, 3);
	pWind->DrawRectangle(wh_x, wh_y + 30, wh_x + wh_w, wh_y + wh_h);

	pWind->SetBrush(DARKRED);
	pWind->SetPen(BLACK, 2);
	pWind->DrawRectangle(wh_x - 5, wh_y, wh_x + wh_w + 5, wh_y + 35);

	pWind->SetBrush(BLACK);
	pWind->SetPen(BLACK, 1);
	int door_x = wh_x + wh_w / 2 - 15;
	pWind->DrawRectangle(door_x, wh_y + 60, door_x + 30, wh_y + wh_h);

	pWind->SetPen(WHITE, 1);
	pWind->SetFont(13, BOLD, BY_NAME, "Arial");
	pWind->DrawString(wh_x + 20, wh_y + 115, "Warehouse");
}

void Game::drawStatusBar() const
{
	clearStatusBar();
	pWind->SetPen(WHITE, 50);
	pWind->SetFont(20, BOLD, BY_NAME, "Arial");

	long currentTime = time(NULL);
	long elapsedSeconds = currentTime - startTime;

	string timerStr = "Timer: " + to_string(elapsedSeconds) + "s";
	string goalStr = "Goal: " + to_string(goal);
	string levelStr = "Level: " + to_string(level);
	string countStr = "Animals: " + to_string(animalCount);

	pWind->DrawString(10, config.windHeight - 40, timerStr);
	pWind->DrawString(150, config.windHeight - 40, goalStr);
	pWind->DrawString(300, config.windHeight - 40, levelStr);
	pWind->DrawString(450, config.windHeight - 40, countStr);
}

window* Game::getWind() const
{
	return pWind;
}

void Game::go() const
{
	int x, y;
	bool isExit = false;

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");

	pWind->SetBuffering(true);

	do
	{
		drawField();
		drawWarehouse();

		gameToolbar->draw();
		gameBudgetbar->draw();

		gameBudgetbar->update();

		drawStatusBar();

		if (pWind->GetMouseClick(x, y) != NO_CLICK)
		{
			if (y >= 0 && y < config.toolBarHeight)
			{
				isExit = gameToolbar->handleClick(x, y);
			}
			else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
			{
				isExit = gameBudgetbar->handleClick(x, y);
			}
		}

		pWind->UpdateBuffer();

	} while (!isExit);
}


