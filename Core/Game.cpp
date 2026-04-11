#include "Game.h"
#include "../Config/GameConfig.h"

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

void Game::drawTimer() const
{
	int minutes = remainingTimeSeconds / 60;
	int seconds = remainingTimeSeconds % 60;

	string timeStr = "Time: ";
	if(minutes < 10) timeStr += "0";
	timeStr += to_string(minutes) + ":";
	if(seconds < 10) timeStr += "0";
	timeStr += to_string(seconds);

	pWind->SetPen(config.penColor, 50); 
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");

	pWind->DrawString(config.windWidth - 200, config.windHeight - (int)(0.85 * config.statusBarHeight), timeStr);
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

	int startTime = time(NULL); // Store the current time in seconds

	do
	{
		int currentTime = time(NULL); // Get current time
		if (currentTime - startTime >= 1) // 1 second has passed
		{
			if (remainingTimeSeconds > 0)
			{
				remainingTimeSeconds--;
			}
			startTime = currentTime;
		}

		if (remainingTimeSeconds <= 0)
		{
			pWind->UpdateBuffer(); 
			pWind->SetPen(RED, 50);
			pWind->SetFont(40, BOLD, BY_NAME, "Arial");
			pWind->DrawString(config.windWidth / 2 - 150, config.windHeight / 2, "TIME'S UP! YOU LOSE!");
			printMessage("Game Over! Click anywhere to exit...");

			drawTimer();

			pWind->WaitMouseClick(x, y); 
			isExit = true;
			break;
		}

		printMessage("Ready...");
		string budget_string = "BUDGET = $" + to_string(budget);
		printBudget(budget_string);
		drawTimer();

		// pWind->GetMouseClick DOES NOT BLOCK. WaitMouseClick blocked.
		clicktype ct = pWind->GetMouseClick(x, y);	

		if (ct != NO_CLICK)
		{
			//if (gameMode == MODE_DSIGN)		//Game is in the Desgin mode
			//{
				//[1] If user clicks on the Toolbar
			if (y >= 0 && y < config.toolBarHeight)
			{
				isExit = gameToolbar->handleClick(x, y);
			}
			else if (y >= config.toolBarHeight && y < 2*config.toolBarHeight)
			{
				isExit = gameBudgetbar->handleClick(x, y);
			}
			//}
		}

		Sleep(15); 

	} while (!isExit);
}


