#include "Game.h"
#include "../Config/GameConfig.h"
#include "Foodarea.h"
#include "Product.h"
#include <random>
#include <ctime>

Game::Game()
{
	budget = 2000;
	level = 1;
	currentLevel = 1;
	gameStartTime = CurrentTime();
	lastWolfSpawnTime = 0;

	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);
	pWind->SetBuffering(true);
	pWind->SetBrush(config.bkGrndColor);
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->DrawRectangle(0, 0, config.windWidth, config.windHeight);

	//2 - create and draw the toolbar
	createToolbar();
	createBudgetbar();
	//3 - create and draw the backgroundPlayingArea


	//4- Create the Warehouse
	point warehousePos;
	warehousePos.x = config.windWidth - 150;
	warehousePos.y = config.windHeight - config.statusBarHeight - 120;
	pWarehouse = new Warehouse(this, warehousePos, 120, 80, 500);

	//5- Create the Plane
	//TODO: Add code to create and draw the Plane

	point eggPos;
	eggPos.x = 100;
	eggPos.y = 200;
	Product* testEgg = new Egg(this, eggPos, 100, 100, "images\\egg.jpg"); // Image path for Egg
	productList.push_back(testEgg);

	point milkPos;
	milkPos.x = 250;
	milkPos.y = 200;
	Product* testMilk = new Milk(this, milkPos, 100, 100, "images\\milk.jpg"); // Image path for Milk
	productList.push_back(testMilk);

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
	delete pWarehouse;
	delete pWind;
}

clicktype Game::getMouseClick(int& x, int& y) const
{
	return pWind->WaitMouseClick(x, y);
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
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(config.windWidth - 500, config.toolBarHeight, config.windWidth, 2 * config.toolBarHeight);
}

void Game::printBudget(string msg) const
{
	clearBudget();

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(config.windWidth - 200, config.toolBarHeight + 10, msg);
}

void Game::drawTimer() const
{
	int minutes = remainingTimeSeconds / 60;
	int seconds = remainingTimeSeconds % 60;

	string timeStr = "Time: ";
	if (minutes < 10) timeStr += "0";
	timeStr += to_string(minutes) + ":";
	if (seconds < 10) timeStr += "0";
	timeStr += to_string(seconds);

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");

	pWind->DrawString(config.windWidth - 200, config.windHeight - (int)(0.85 * config.statusBarHeight), timeStr);
}

void Game::clearStatusBar() const
{
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
	clearStatusBar();
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
	const unsigned long currentTime = currentGameTime;
	const unsigned long elapsedSeconds = currentTime;
	currentLevel = static_cast<int>(elapsedSeconds / 30) + 1;
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

	if (pWarehouse) {
		pWarehouse->Reset();
	}

	budget = 2000;
	level = 1;
	currentLevel = 1;
	remainingTimeSeconds = 120;
	animalCount = 0;
	gameStartTime = CurrentTime();
	currentGameTime = 0;
	lastWolfSpawnTime = 0;

	startTime = time(NULL);
	gameToolbar->draw();
	gameBudgetbar->draw();
	clearStatusBar();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage("Ready...");
	isPaused = false;
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

	unsigned long elapsedSeconds = currentGameTime;

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

void Game::setPaused(bool pause)
{
	isPaused = pause;
	if (isPaused) {
		printMessage("Game Paused. Click Resume to continue.");
	}
	else {
		printMessage("Game Resumed.");
	}
}

bool Game::isGamePaused() const
{
	return isPaused;
}

void Game::go()
{
	int x, y;
	bool isExit = false;

	unsigned long lastSecondTick = CurrentTime();

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");

	pWind->SetBuffering(true);

	do
	{
		unsigned long now = CurrentTime();

		// 1. Timer Logic: Check if 1000ms (1 second) has passed
		if (!isPaused && now - lastSecondTick >= 1000)
		{
			currentGameTime++;
			if (remainingTimeSeconds > 0)
			{
				remainingTimeSeconds--;
			}
			lastSecondTick = now;
		}

	
		if (remainingTimeSeconds <= 0)
		{
			isPaused = true;
			pWind->UpdateBuffer();
			pWind->SetPen(RED, 50);
			pWind->SetFont(40, BOLD, BY_NAME, "Arial");
			pWind->DrawString(config.windWidth / 2 - 150, config.windHeight / 2, "TIME'S UP! YOU LOSE!");
			printMessage("Game Over! Click anywhere to exit...");
			drawTimer();
			pWind->UpdateBuffer(); 
			pWind->WaitMouseClick(x, y);
			isExit = true;
			break;
		}

		if (!isPaused)
		{
			drawField();

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

			for (size_t i = 0; i < animalList.size(); i++) {
				if (animalList[i]) {
					animalList[i]->moveStep();
					animalList[i]->draw();

					if (animalList[i]->checkProduction()) {
						point dropPos = animalList[i]->getPos();
						
						budget += 50;
					}
				}
			}

			for (size_t i = 0; i < wolves.size(); i++) {
				wolves[i]->moveStep();
				wolves[i]->draw();
			}

			for (size_t i = 0; i < productList.size(); i++) {
				if (productList[i]) productList[i]->draw();
			}

			if (pWarehouse) pWarehouse->draw();

			gameBudgetbar->update();

			if (currentGameTime - lastWolfSpawnTime >= 30) {
				generateRandomWolves();
				lastWolfSpawnTime = currentGameTime;

				level = currentLevel;

				int newTimeLimit = 120 - (level * 15);

				if (newTimeLimit < 30) {
					newTimeLimit = 30;
				}

				remainingTimeSeconds = newTimeLimit;

				drawTimer();
				drawStatusBar();
				pWind->UpdateBuffer();

				printMessage("Level " + to_string(level) + "! Time decreases to " + to_string(remainingTimeSeconds) + "s!");
			}
		}
		else {
			drawField();
			for (auto food : foodList) if (food) food->draw();
			for (auto animal : animalList) if (animal) animal->draw();
			for (auto wolf : wolves) if (wolf) wolf->draw();
			for (auto product : productList) if (product) product->draw();
			if (pWarehouse) pWarehouse->draw();
		}

		
		gameToolbar->draw();
		gameBudgetbar->draw();
		gameBudgetbar->update();
		drawStatusBar();
		drawTimer();
		printBudget("BUDGET = $" + to_string(budget));

		clicktype click = pWind->GetMouseClick(x, y);
		if (click != NO_CLICK)
		{
			
			if (pWarehouse != nullptr) {
				int wx = config.windWidth - 150;
				int wy = config.windHeight - config.statusBarHeight - 120;
				int ww = 120;
				int wh = 80;
		
				if (x >= wx && x <= wx + ww && y >= wy && y <= wy + wh) {
					showWarehouse();
				}
			}

			if (y >= 0 && y < config.toolBarHeight) {
				isExit = gameToolbar->handleClick(x, y);
			}
			else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight) {
				isExit = gameBudgetbar->handleClick(x, y);
			}
			else if (y >= 2 * config.toolBarHeight && y < config.windHeight - config.statusBarHeight) {
				if (!isPaused) {
					bool itemCollected = false;

					for (int i = 0; i < productList.size(); i++) {
						if (productList[i] != nullptr && productList[i]->isClicked(x, y)) {
							if (pWarehouse && pWarehouse->StoreItem(productList[i]->getType())) {
								delete productList[i];  
								productList.erase(productList.begin() + i); 
								itemCollected = true;
							}
							else {
								printMessage("Warehouse is full!"); 
							}
							break; 
						}
					}

					if (!itemCollected && budget >= 100) {
						point p; p.x = x - 25; p.y = y - 25;
						FoodArea* pNewFood = new FoodArea(this, p, 50, 50, "images\\grass.jpg", 100);
						foodList.push_back(pNewFood);
						budget -= 100;
					}
				}
			}
		}

		pWind->UpdateBuffer();
		Sleep(30); 

	} while (!isExit);
}

void Game::showWarehouse()
{
	int wWidth = 400;
	int wHeight = 300;

	window* pWarehouseWind = new window(wWidth, wHeight, config.wx + 50, config.wy + 50);
	pWarehouseWind->ChangeTitle("Warehouse Inventory");

	pWarehouseWind->SetPen(LIGHTGRAY, 1);
	pWarehouseWind->DrawRectangle(0, 0, wWidth, wHeight);

	pWarehouseWind->SetPen(BLACK, 50);
	pWarehouseWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWarehouseWind->DrawString(20, 20, "- Warehouse Inventory -");

	pWarehouseWind->SetFont(18, BOLD, BY_NAME, "Arial");

	int eggCount;
	int milkCount;
	int totalItems;
	int capacity;

	if (pWarehouse != nullptr)
	{
		eggCount = pWarehouse->GetItemCount(ProductType::EGG);
		milkCount = pWarehouse->GetItemCount(ProductType::MILK);
		totalItems = pWarehouse->GetTotalItems();
		capacity = pWarehouse->GetCapacity();
	}
	else
	{
		eggCount = 0;
		milkCount = 0;
		totalItems = 0;
		capacity = 0;
	}

	pWarehouseWind->DrawString(20, 80, "Eggs: " + to_string(eggCount) + "   [$" + to_string(Warehouse::GetProductPrice(ProductType::EGG)) + " each]");
	pWarehouseWind->DrawString(20, 110, "Milk: " + to_string(milkCount) + "   [$" + to_string(Warehouse::GetProductPrice(ProductType::MILK)) + " each]");

	int fishCount;
	if (pWarehouse != nullptr)
	{
		fishCount = pWarehouse->GetItemCount(ProductType::FISH);
	}
	else
	{
		fishCount = 0;
	}

	pWarehouseWind->DrawString(20, 140, "Fish: " + to_string(fishCount) + "   [$" + to_string(Warehouse::GetProductPrice(ProductType::FISH)) + " each]");

	pWarehouseWind->SetPen(RED, 50);
	pWarehouseWind->DrawString(20, 170, "Total Storage: " + to_string(totalItems) + " / " + to_string(capacity));

	pWarehouseWind->SetPen(DARKRED, 50);
	pWarehouseWind->DrawString(20, wHeight - 40, "Click anywhere inside to close...");

	int x, y;
	pWarehouseWind->WaitMouseClick(x, y);

	delete pWarehouseWind;
}

void Game::addProduct(Product* p) {
    if (p) productList.push_back(p);
}

