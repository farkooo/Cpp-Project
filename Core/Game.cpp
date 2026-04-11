#include "Game.h"
#include "../Config/GameConfig.h"
#include "Foodarea.h"
#include "Product.h"
#include <random>
#include <ctime>

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
	startTime = time(NULL);

	clearStatusBar();
	printBudget("BUDGET = $" + to_string(budget));
	drawStatusBar();
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

	if (gameBudgetbar) {
		gameBudgetbar->reset();
	}

	budget = 2000;
	currentLevel = 1;
	animalCount = 0;
	gameStartTime = CurrentTime();
	lastWolfSpawnTime = gameStartTime;

	startTime = time(NULL);
	gameToolbar->draw();
	gameBudgetbar->draw();
	clearStatusBar();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage("Ready...");
	pWind->UpdateBuffer();
}
void Game::drawField() const
{
	pWind->SetPen(BROWN, 10);
	pWind->SetBrush(GREEN);
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);
}

void Game::drawStatusBar() const
{
	clearStatusBar();
	pWind->SetPen(WHITE, 50);
	pWind->SetFont(20, BOLD, BY_NAME, "Arial");

	time_t currentTime = time(NULL);
	long elapsedSeconds = (long)(currentTime - startTime);

	if (elapsedSeconds < 0 || elapsedSeconds > 100000) elapsedSeconds = 0;

	string timerStr = "Timer: " + to_string(elapsedSeconds) + "s";
	string goalStr = "Goal: " + to_string(goal);
	string levelStr = "Level: " + to_string(level);
	string countStr = "Animals: " + to_string(animalCount);

	pWind->DrawString(10, config.windHeight - 40, timerStr);
	pWind->DrawString(180, config.windHeight - 40, goalStr);
	pWind->DrawString(330, config.windHeight - 40, levelStr);
	pWind->DrawString(480, config.windHeight - 40, countStr);
}

window* Game::getWind() const
{
	return pWind;
}

void Game::go()
{
	int x, y;
	bool isExit = false;

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");
	pWind->SetBuffering(true);


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

		pWind->SetBrush(config.bkGrndColor);
		pWind->SetPen(config.bkGrndColor, 1);
		pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

		
		drawField();
		const unsigned long currentTime = CurrentTime();
		if (currentTime - lastWolfSpawnTime >= 30000UL)
		{
			generateRandomWolves();
			lastWolfSpawnTime = currentTime;
		}
		gameBudgetbar->update();
		drawStatusBar();
		gameToolbar->draw();
		gameBudgetbar->draw();
		printBudget("BUDGET = $" + to_string(budget));


		for (size_t i = 0; i < wolves.size(); i++)
		{
			wolves[i]->moveStep();
			wolves[i]->draw();
		}

		

		clicktype click = pWind->GetMouseClick(x, y);

		if (click != NO_CLICK)
		{
			if (y >= 0 && y < config.toolBarHeight)
			{

				isExit = gameToolbar->handleClick(x, y);
			}
			else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
			
				isExit = gameBudgetbar->handleClick(x, y);
			}


		Pause(50);
		pWind->UpdateBuffer();

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
