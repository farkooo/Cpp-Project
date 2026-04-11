#include "Game.h"
#include "../Config/GameConfig.h"
#include "Foodarea.h"
#include "Product.h"
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
	pWind->DrawRectangle(config.windWidth - 500, config.toolBarHeight, config.windWidth, 2 * config.toolBarHeight);
}

void Game::printBudget(string msg) const
{
	clearBudget();	//First clear the status bar

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(config.windWidth - 200, config.toolBarHeight + 10, msg);

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
	int x, y;
	bool isExit = false;

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy - - - - - - - - - -");

	do
	{
		// 1. مسح الشاشة (السبورة) في كل لفة
		pWind->SetPen(config.bkGrndColor, 1);
		pWind->SetBrush(config.bkGrndColor);
		pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

		// 2. تحديث الرسائل والميزانية
		string budget_string = "BUDGET = $" + to_string(budget);
		printBudget(budget_string);

		// --- 🌟 أولاً: إدارة العشب (رسم ومسح) ---
		for (size_t i = 0; i < foodList.size(); i++) {
			if (foodList[i]) {
				foodList[i]->draw();
				if (foodList[i]->isEmpty()) {
					delete foodList[i];
					foodList.erase(foodList.begin() + i);
					i--;
				}
			}
		}

		// --- 🌟 ثانياً: إدارة الحيوانات والإنتاج (Task 19 & 20) ---
		for (size_t i = 0; i < animalList.size(); i++) {
			if (animalList[i]) {
				animalList[i]->draw();
				animalList[i]->moveStep(); // حركة الحيوان

				// فحص الإنتاج
				if (animalList[i]->checkProduction()) {
					point dropPos = animalList[i]->getPos();

					// بنرمي المنتج (استخدمنا المسار النسبي اللي اتفقنا عليه)
					Product* pNew = new Egg(this, dropPos, 30, 30, "images\\egg.jpg");
					productList.push_back(pNew);

					budget += 50; // مكافأة الإنتاج
					printMessage("Animal Produced! +$50");
				}
			}
		}

		// --- 🌟 ثالثاً: رسم المنتجات (البيض واللبن) ---
		for (size_t i = 0; i < productList.size(); i++) {
			if (productList[i]) {
				productList[i]->draw();
			}
		}

		// --- 🌟 رابعاً: التعامل مع ضغطات الماوس ---
		if (pWind->GetMouseClick(x, y) != NO_CLICK)
		{
			if (y >= 2 * config.toolBarHeight && y < config.windHeight - config.statusBarHeight)
			{
				if (budget >= 20) {
					point p; p.x = x; p.y = y;
					FoodArea* pNewFood = new FoodArea(this, p, 50, 50, GREEN, BLACK, 100);
					foodList.push_back(pNewFood);
					budget -= 20;
				}
			}
			else if (y >= 0 && y < config.toolBarHeight)
			{
				isExit = gameToolbar->handleClick(x, y);
			}
			else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
			{
				isExit = gameBudgetbar->handleClick(x, y);
			}
		}

		Sleep(30);

	} while (!isExit);
}