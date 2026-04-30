#include "Game.h"
#include "../Config/GameConfig.h"
#include "Foodarea.h"
#include "Product.h"
#include <random>
#include <ctime>

namespace
{
	struct ButtonRect
	{
		int left;
		int top;
		int right;
		int bottom;
	};

	bool IsInsideRect(int x, int y, const ButtonRect& rect)
	{
		return x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom;
	}

	string GetProductDisplayName(ProductType productType)
	{
		switch (productType)
		{
		case ProductType::EGG:
			return "Eggs";
		case ProductType::MILK:
			return "Milk";
		case ProductType::FISH:
			return "Fish";
		default:
			return "Product";
		}
	}

	void DrawButton(window* pWindow, const ButtonRect& rect, const string& label, bool enabled)
	{
		pWindow->SetPen(enabled ? DARKGREEN : DARKGRAY, 2);
		pWindow->SetBrush(enabled ? color(225, 245, 225) : color(220, 220, 220));
		pWindow->DrawRectangle(rect.left, rect.top, rect.right, rect.bottom);

		pWindow->SetPen(enabled ? BLACK : GRAY, 50);
		pWindow->SetFont(14, BOLD, BY_NAME, "Arial");
		pWindow->DrawString(rect.left + 8, rect.top + 7, label);
	}

	void DrawWarehouseProductRow(window* pWindow, int y, const string& name, int count, int price,
		const ButtonRect& sellOneButton, const ButtonRect& sellAllButton)
	{
		pWindow->SetPen(BLACK, 50);
		pWindow->SetFont(17, BOLD, BY_NAME, "Arial");
		pWindow->DrawString(20, y + 6, name);
		pWindow->DrawString(105, y + 6, "Qty: " + to_string(count));
		pWindow->DrawString(190, y + 6, "$" + to_string(price));

		const bool canSell = count > 0;
		DrawButton(pWindow, sellOneButton, "Sell 1", canSell);
		DrawButton(pWindow, sellAllButton, "Sell All", canSell);
	}

	const int LEVEL_GOALS[] = { 2500, 3500, 5000, 7000, 9500 };
	const int LEVEL_GOAL_COUNT = sizeof(LEVEL_GOALS) / sizeof(LEVEL_GOALS[0]);
	const int MAX_WOLVES_PER_SPAWN = 6;
	const int MAX_WOLF_SPEED = 12;
}

Game::Game()
{
	budget = 2000;
	level = 1;
	currentLevel = 1;
	goal = GetLevelGoal(level);
	remainingTimeSeconds = GetLevelTimeLimit(level);
	gameStartTime = CurrentTime();
	lastWolfSpawnTime = 0;

	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);
	pWind->SetBuffering(true);
	pWind->SetBrush(config.bkGrndColor);
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->DrawRectangle(0, 0, config.windWidth, config.windHeight);

	createToolbar();
	createBudgetbar();

	point warehousePos;
	warehousePos.x = config.windWidth - 150;
	warehousePos.y = config.windHeight - config.statusBarHeight - 120;
	pWarehouse = new Warehouse(this, warehousePos, 120, 80, 500);

	point eggPos;
	eggPos.x = 100;
	eggPos.y = 200;
	Product* testEgg = new Egg(this, eggPos, 100, 100, "images\\egg.jpg");
	productList.push_back(testEgg);

	point milkPos;
	milkPos.x = 250;
	milkPos.y = 200;
	Product* testMilk = new Milk(this, milkPos, 100, 100, "images\\milk.jpg");
	productList.push_back(testMilk);

	startTime = time(NULL);

	clearStatusBar();
	printBudget("BUDGET = $" + to_string(budget));
	drawStatusBar();
}

Game::~Game()
{
	for (size_t i = 0; i < animalList.size(); i++) delete animalList[i];
	for (size_t i = 0; i < foodList.size(); i++) delete foodList[i];
	for (size_t i = 0; i < productList.size(); i++) delete productList[i];
	for (size_t i = 0; i < chicks.size(); i++) delete chicks[i];
	for (size_t i = 0; i < wolves.size(); i++) delete wolves[i];

	delete gameToolbar;
	delete gameBudgetbar;
	delete pWarehouse;
	delete pWind;
}

clicktype Game::getMouseClick(int& x, int& y) const { return pWind->WaitMouseClick(x, y); }

string Game::getSrting() const
{
	string Label;
	char Key;
	keytype ktype;
	pWind->FlushKeyQueue();
	while (1)
	{
		ktype = pWind->WaitKeyPress(Key);
		if (ktype == ESCAPE) return "";
		if (Key == 13) return Label;
		if (Key == 8) {
			if (Label.size() > 0) Label.resize(Label.size() - 1);
			else Key = '\0';
		}
		else Label += Key;
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
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);
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

	const int wolfWidth = 70;
	const int wolfHeight = 70;

	std::mt19937 generator(static_cast<unsigned int>(currentTime + (currentLevel * 97) + (wolves.size() * 31)));
	std::uniform_int_distribution<int> xDist(30, config.windWidth - wolfWidth - 30);
	std::uniform_int_distribution<int> yDist((2 * config.toolBarHeight) + 30, config.windHeight - config.statusBarHeight - wolfHeight - 30);

	point position;
	position.x = xDist(generator);
	position.y = yDist(generator);

	drawWolf(position, wolfWidth, wolfHeight, GetWolfSpeed());
}

int Game::getCurrentLevel() const { return currentLevel; }

void Game::restartGame()
{
	clearPlayingArea();
	for (size_t i = 0; i < animalList.size(); i++) delete animalList[i];
	animalList.clear();
	for (size_t i = 0; i < foodList.size(); i++) delete foodList[i];
	foodList.clear();
	for (size_t i = 0; i < productList.size(); i++) delete productList[i];
	productList.clear();
	for (size_t i = 0; i < wolves.size(); i++) delete wolves[i];
	wolves.clear();
	for (size_t i = 0; i < chicks.size(); i++) delete chicks[i];
	chicks.clear();

	if (gameBudgetbar) gameBudgetbar->reset();
	if (pWarehouse) pWarehouse->Reset();

	budget = 2000;
	level = 1;
	currentLevel = 1;
	goal = GetLevelGoal(level);
	remainingTimeSeconds = GetLevelTimeLimit(level);
	animalCount = 0;
	gameStartTime = CurrentTime();
	currentGameTime = 0;
	lastWolfSpawnTime = 0;

	startTime = time(NULL);

	// Force redraw UI
	if (gameToolbar) gameToolbar->draw();
	if (gameBudgetbar) gameBudgetbar->draw();
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

	string timerStr = "Timer: " + to_string(currentGameTime) + "s";
	string goalStr = "Goal: $" + to_string(goal);
	string levelStr = "Level: " + to_string(level);
	string countStr = "Animals: " + to_string(animalCount);

	pWind->DrawString(10, config.windHeight - 40, timerStr);
	pWind->DrawString(180, config.windHeight - 40, goalStr);
	pWind->DrawString(330, config.windHeight - 40, levelStr);
	pWind->DrawString(480, config.windHeight - 40, countStr);
}

window* Game::getWind() const { return pWind; }

void Game::setPaused(bool pause)
{
	isPaused = pause;
	if (isPaused) printMessage("Game Paused. Click Resume to continue.");
	else printMessage("Game Resumed.");
}

bool Game::isGamePaused() const { return isPaused; }

int Game::GetLevelGoal(int levelNumber) const
{
	if (levelNumber <= LEVEL_GOAL_COUNT) return LEVEL_GOALS[levelNumber - 1];

	const int extraLevels = levelNumber - LEVEL_GOAL_COUNT;
	return LEVEL_GOALS[LEVEL_GOAL_COUNT - 1] + (extraLevels * 3000);
}

int Game::GetLevelTimeLimit(int levelNumber) const
{
	const int timeLimit = 120 - ((levelNumber - 1) * 15);
	return timeLimit < 30 ? 30 : timeLimit;
}

unsigned long Game::GetWolfSpawnInterval() const
{
	return 30;
}

int Game::GetWolfCountPerSpawn() const
{
	const int count = level < 1 ? 1 : level;
	return count > MAX_WOLVES_PER_SPAWN ? MAX_WOLVES_PER_SPAWN : count;
}

int Game::GetWolfSpeed() const
{
	const int speed = 1 + ((level - 1) * 2);
	return speed > MAX_WOLF_SPEED ? MAX_WOLF_SPEED : speed;
}

void Game::updateWolfDifficulty(int currentLevel)
{
	currentLevel = currentLevel < 1 ? 1 : currentLevel;
	const int newSpeed = 1 + ((currentLevel - 1) * 2);
	const int cappedSpeed = newSpeed > MAX_WOLF_SPEED ? MAX_WOLF_SPEED : newSpeed;

	for (size_t i = 0; i < wolves.size(); i++) {
		if (wolves[i]) wolves[i]->setSpeed(cappedSpeed);
	}
}

void Game::spawnWolfEveryInterval()
{
	if (currentGameTime - lastWolfSpawnTime < GetWolfSpawnInterval()) return;

	const int wolvesToSpawn = GetWolfCountPerSpawn();
	for (int i = 0; i < wolvesToSpawn; i++) {
		generateRandomWolves();
	}

	lastWolfSpawnTime = currentGameTime;
}

void Game::ApplyLevelUp()
{
	level++;
	currentLevel = level;
	goal = GetLevelGoal(level);
	remainingTimeSeconds = GetLevelTimeLimit(level);
	lastWolfSpawnTime = currentGameTime;
	updateWolfDifficulty(currentLevel);

	printMessage("Level Up! Level " + to_string(level) + " - Goal: $" + to_string(goal));
}

bool Game::checkLevelProgress(int currentBudget, int currentLevel)
{
	if (currentLevel != level) return false;
	if (currentBudget < goal) return false;

	ApplyLevelUp();
	return true;
}

bool Game::SellWarehouseProduct(ProductType productType, int quantityToSell)
{
	if (!pWarehouse || quantityToSell <= 0) return false;

	const int availableCount = pWarehouse->GetItemCount(productType);
	if (availableCount <= 0) return false;

	const int soldCount = quantityToSell < availableCount ? quantityToSell : availableCount;
	if (!pWarehouse->RemoveItem(productType, soldCount)) return false;

	budget += Warehouse::GetProductPrice(productType) * soldCount;
	checkLevelProgress(budget, level);
	printBudget("BUDGET = $" + to_string(budget));
	if (pWind) pWind->UpdateBuffer();

	return true;
}

bool Game::isGamePaused() const { return isPaused; }

void Game::go()
{
	int x, y;
	bool isExit = false;

	unsigned long lastSecondTick = CurrentTime();
	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");
	pWind->SetBuffering(true);

	clearPlayingArea();
	if (gameToolbar) gameToolbar->draw();
	if (gameBudgetbar) gameBudgetbar->draw();
	drawStatusBar();
	drawTimer();
	printBudget("BUDGET = $" + to_string(budget));

	int lastDrawnBudget = budget;
	int lastDrawnSeconds = remainingTimeSeconds;
	int lastDrawnLevel = level;
	int lastDrawnAnimalCount = animalCount;

	do
	{
		unsigned long loopStartTime = CurrentTime();
		bool uiNeedsUpdate = false;

		if (!isPaused && loopStartTime - lastSecondTick >= 1000)
		{
			currentGameTime++;
			if (remainingTimeSeconds > 0) remainingTimeSeconds--;
			lastSecondTick = loopStartTime;
			uiNeedsUpdate = true; 
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
						budget += 50;
						if (checkLevelProgress(budget, level)) {
							uiNeedsUpdate = true;
						}
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

			spawnWolfEveryInterval();
		}
		else {
			drawField();
			for (auto food : foodList) if (food) food->draw();
			for (auto animal : animalList) if (animal) animal->draw();
			for (auto wolf : wolves) if (wolf) wolf->draw();
			for (auto product : productList) if (product) product->draw();
			if (pWarehouse) pWarehouse->draw();
		}

		if (gameBudgetbar) gameBudgetbar->update();

		// SMART UI REDRAW: Only execute heavy GDI calls if necessary
		if (lastDrawnBudget != budget || lastDrawnSeconds != remainingTimeSeconds ||
			lastDrawnLevel != level || lastDrawnAnimalCount != animalCount)
		{
			uiNeedsUpdate = true;
		}

		if (uiNeedsUpdate) {
			drawStatusBar();
			drawTimer();
			printBudget("BUDGET = $" + to_string(budget));
			lastDrawnBudget = budget;
			lastDrawnSeconds = remainingTimeSeconds;
			lastDrawnLevel = level;
			lastDrawnAnimalCount = animalCount;
		}

		clicktype click = pWind->GetMouseClick(x, y);
		if (click != NO_CLICK)
		{
			if (pWarehouse != nullptr) {
				int wx = config.windWidth - 150;
				int wy = config.windHeight - config.statusBarHeight - 120;
				if (x >= wx && x <= wx + 120 && y >= wy && y <= wy + 80) {
					showWarehouse();
					if (gameToolbar) gameToolbar->draw();
					if (gameBudgetbar) gameBudgetbar->draw();
					drawStatusBar();
					drawTimer();
					printBudget("BUDGET = $" + to_string(budget));
					lastDrawnBudget = budget;
					lastDrawnSeconds = remainingTimeSeconds;
					lastDrawnLevel = level;
					lastDrawnAnimalCount = animalCount;
					pWind->FlushMouseQueue();
					pWind->UpdateBuffer();
					continue;
					uiNeedsUpdate = true;
				}
			}

			if (y >= 0 && y < config.toolBarHeight) {
				if (gameToolbar) {
					isExit = gameToolbar->handleClick(x, y);
					gameToolbar->draw();
				}
			}
			else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight) {
				if (gameBudgetbar) {
					isExit = gameBudgetbar->handleClick(x, y);
					gameBudgetbar->draw();
				}
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

		unsigned long timeTaken = CurrentTime() - loopStartTime;
		if (timeTaken < 30) {
			Sleep(30 - timeTaken);
		}
		else {
			Sleep(1);
		}

	} while (!isExit);
}

void Game::showWarehouse()
{
	const int wWidth = 560;
	const int wHeight = 330;

	window* pWarehouseWind = new window(wWidth, wHeight, config.wx + 50, config.wy + 50);
	pWarehouseWind->ChangeTitle("Warehouse Inventory");
	pWarehouseWind->SetBuffering(true);

	const ButtonRect eggSellOne = { 300, 82, 370, 112 };
	const ButtonRect eggSellAll = { 385, 82, 480, 112 };
	const ButtonRect milkSellOne = { 300, 122, 370, 152 };
	const ButtonRect milkSellAll = { 385, 122, 480, 152 };
	const ButtonRect fishSellOne = { 300, 162, 370, 192 };
	const ButtonRect fishSellAll = { 385, 162, 480, 192 };
	const ButtonRect closeButton = { 420, wHeight - 48, 520, wHeight - 18 };

	string feedback = "Select a sell button.";
	bool isOpen = true;

	while (isOpen)
	{
		const int eggCount = pWarehouse ? pWarehouse->GetItemCount(ProductType::EGG) : 0;
		const int milkCount = pWarehouse ? pWarehouse->GetItemCount(ProductType::MILK) : 0;
		const int fishCount = pWarehouse ? pWarehouse->GetItemCount(ProductType::FISH) : 0;
		const int totalItems = pWarehouse ? pWarehouse->GetTotalItems() : 0;
		const int capacity = pWarehouse ? pWarehouse->GetCapacity() : 0;

		pWarehouseWind->SetPen(LIGHTGRAY, 1);
		pWarehouseWind->SetBrush(color(245, 245, 240));
		pWarehouseWind->DrawRectangle(0, 0, wWidth, wHeight);

		pWarehouseWind->SetPen(BLACK, 50);
		pWarehouseWind->SetFont(24, BOLD, BY_NAME, "Arial");
		pWarehouseWind->DrawString(20, 20, "- Warehouse Inventory -");

		pWarehouseWind->SetPen(DARKBLUE, 50);
		pWarehouseWind->SetFont(17, BOLD, BY_NAME, "Arial");
		pWarehouseWind->DrawString(20, 52, "Budget: $" + to_string(budget));

		DrawWarehouseProductRow(pWarehouseWind, 80, GetProductDisplayName(ProductType::EGG), eggCount,
			Warehouse::GetProductPrice(ProductType::EGG), eggSellOne, eggSellAll);
		DrawWarehouseProductRow(pWarehouseWind, 120, GetProductDisplayName(ProductType::MILK), milkCount,
			Warehouse::GetProductPrice(ProductType::MILK), milkSellOne, milkSellAll);
		DrawWarehouseProductRow(pWarehouseWind, 160, GetProductDisplayName(ProductType::FISH), fishCount,
			Warehouse::GetProductPrice(ProductType::FISH), fishSellOne, fishSellAll);

		pWarehouseWind->SetPen(RED, 50);
		pWarehouseWind->SetFont(17, BOLD, BY_NAME, "Arial");
		pWarehouseWind->DrawString(20, 215, "Total Storage: " + to_string(totalItems) + " / " + to_string(capacity));

		pWarehouseWind->SetPen(DARKGREEN, 50);
		pWarehouseWind->SetFont(16, BOLD, BY_NAME, "Arial");
		pWarehouseWind->DrawString(20, 252, feedback);

		DrawButton(pWarehouseWind, closeButton, "Close", true);
		pWarehouseWind->UpdateBuffer();

		int x, y;
		pWarehouseWind->WaitMouseClick(x, y);

		ProductType selectedProduct = ProductType::EGG;
		int sellQuantity = 0;

		if (IsInsideRect(x, y, closeButton))
		{
			isOpen = false;
		}
		else if (IsInsideRect(x, y, eggSellOne))
		{
			selectedProduct = ProductType::EGG;
			sellQuantity = 1;
		}
		else if (IsInsideRect(x, y, eggSellAll))
		{
			selectedProduct = ProductType::EGG;
			sellQuantity = eggCount;
		}
		else if (IsInsideRect(x, y, milkSellOne))
		{
			selectedProduct = ProductType::MILK;
			sellQuantity = 1;
		}
		else if (IsInsideRect(x, y, milkSellAll))
		{
			selectedProduct = ProductType::MILK;
			sellQuantity = milkCount;
		}
		else if (IsInsideRect(x, y, fishSellOne))
		{
			selectedProduct = ProductType::FISH;
			sellQuantity = 1;
		}
		else if (IsInsideRect(x, y, fishSellAll))
		{
			selectedProduct = ProductType::FISH;
			sellQuantity = fishCount;
		}

		if (sellQuantity > 0)
		{
			const int saleValue = Warehouse::GetProductPrice(selectedProduct) * sellQuantity;
			if (SellWarehouseProduct(selectedProduct, sellQuantity))
			{
				feedback = "Sold " + to_string(sellQuantity) + " " + GetProductDisplayName(selectedProduct)
					+ " for $" + to_string(saleValue) + ".";
			}
			else
			{
				feedback = GetProductDisplayName(selectedProduct) + " is out of stock.";
			}
		}
		else if (!IsInsideRect(x, y, closeButton))
		{
			feedback = "Use a Sell button or Close.";
		}
	}

	delete pWarehouseWind;
}

void Game::addProduct(Product* p) {
	if (p) productList.push_back(p);
}
