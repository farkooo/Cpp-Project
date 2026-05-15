#include "Game.h"
#include "../Config/GameConfig.h"
#include "AudioManager.h"
#include "Foodarea.h"
#include "Product.h"
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>

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

	const int LEVEL_GOALS[] = { 2100, 2600, 3000, 3500, 3900 };
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

	audioManager = new AudioManager();
	audioManager->SetLoop(true);
	audioManager->SetVolume(0.6f);
	audioManager->PlayBackgroundMusic("audio\\Coolest_Farm_music.wav");

	point warehousePos;
	warehousePos.x = config.windWidth - 150;
	warehousePos.y = config.windHeight - config.statusBarHeight - 120;
	pWarehouse = new Warehouse(this, warehousePos, 120, 80, 500);

	startTime = time(NULL);

	clearStatusBar();
	printBudget("BUDGET = $" + to_string(budget));
	drawStatusBar();
	promptUsername();
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
	delete audioManager;
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
		printMessage("Enter Username (Press Enter to confirm): " + Label);
		pWind->UpdateBuffer();
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
	do {
		position.x = xDist(generator);
		position.y = yDist(generator);
	} while (position.x < 300 && position.y + wolfHeight > config.windHeight - config.statusBarHeight - 300);

	drawWolf(position, wolfWidth, wolfHeight, GetWolfSpeed());
}

int Game::getCurrentLevel() const { return currentLevel; }

void Game::promptUsername()
{
	printMessage("Enter Username (Press Enter to confirm): ");
	pWind->UpdateBuffer();
	playerName = getSrting();
	if (playerName.empty()) {
		playerName = "Guest";
	}
	printMessage("Welcome, " + playerName + "! Ready to farm?");

	pWind->UpdateBuffer();
}

void Game::updateAndDisplayLeaderboard()
{
	std::map<std::string, int> scores;
	std::ifstream inFile("leaderboard.txt");
	std::string name;
	int score;

	if (inFile.is_open()) {
		while (inFile >> name >> score) {
			scores[name] = score;
		}
		inFile.close();
	}

	
	if (scores.find(playerName) == scores.end() || budget > scores[playerName]) {
		scores[playerName] = budget;
	}


	std::vector<std::pair<std::string, int>> sortedScores(scores.begin(), scores.end());
	std::sort(sortedScores.begin(), sortedScores.end(),
		[](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
			return a.second > b.second; 
		}
	);

	
	std::ofstream outFile("leaderboard.txt");
	if (outFile.is_open()) {
		for (const auto& pair : sortedScores) {
			outFile << pair.first << " " << pair.second << "\n";
		}
		outFile.close();
	}


	int wWidth = 400;
	int wHeight = 500;
	window* pLeaderboardWind = new window(wWidth, wHeight, config.wx + 100, config.wy + 50);
	pLeaderboardWind->ChangeTitle("Leaderboard");

	pLeaderboardWind->SetPen(LIGHTGRAY, 1);
	pLeaderboardWind->SetBrush(color(240, 240, 250));
	pLeaderboardWind->DrawRectangle(0, 0, wWidth, wHeight);

	pLeaderboardWind->SetPen(DARKBLUE, 50);
	pLeaderboardWind->SetFont(30, BOLD, BY_NAME, "Arial");
	pLeaderboardWind->DrawString(90, 20, "TOP FARMERS");

	pLeaderboardWind->SetPen(BLACK, 50);
	pLeaderboardWind->SetFont(20, BOLD, BY_NAME, "Arial");

	
	int yOffset = 80;
	int displayCount = (std::min)((int)sortedScores.size(), 10);
	for (int i = 0; i < displayCount; i++) {
		string rank = to_string(i + 1) + ". ";
		string entryText = rank + sortedScores[i].first + " - $" + to_string(sortedScores[i].second);

		
		if (sortedScores[i].first == playerName) {
			pLeaderboardWind->SetPen(RED, 50);
		}
		else {
			pLeaderboardWind->SetPen(BLACK, 50);
		}

		pLeaderboardWind->DrawString(50, yOffset, entryText);
		yOffset += 35;
	}

	pLeaderboardWind->SetPen(DARKGRAY, 50);
	pLeaderboardWind->SetFont(16, BOLD, BY_NAME, "Arial");
	pLeaderboardWind->DrawString(90, wHeight - 40, "Click anywhere to close...");

	pLeaderboardWind->UpdateBuffer();

	int cx, cy;
	pLeaderboardWind->WaitMouseClick(cx, cy); 
	delete pLeaderboardWind;
}

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
	grassCount = 0;
	gameStartTime = CurrentTime();
	currentGameTime = 0;
	lastWolfSpawnTime = 0;

	startTime = time(NULL);

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

	pWind->SetPen(GRAY, 10);
	pWind->SetBrush(BLUE);
	pWind->DrawRectangle(0, config.windHeight - config.statusBarHeight - 300, 300, config.windHeight - config.statusBarHeight);
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
	string grassStr = "Grass: " + to_string(grassCount);

	pWind->DrawString(10, config.windHeight - 40, timerStr);
	pWind->DrawString(180, config.windHeight - 40, goalStr);
	pWind->DrawString(330, config.windHeight - 40, levelStr);
	pWind->DrawString(480, config.windHeight - 40, countStr);
	pWind->DrawString(630, config.windHeight - 40, grassStr);
}

window* Game::getWind() const { return pWind; }

void Game::setPaused(bool pause)
{

	isPaused = pause;
	if (isPaused) {
		audioManager->PauseBackgroundMusic();
		printMessage("Game Paused. Click Resume to continue.");
	}
	else {
		audioManager->ResumeBackgroundMusic();
		printMessage("Game Resumed.");
	}
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
			Sleep(1500);
			updateAndDisplayLeaderboard();
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
						grassCount--;
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

			BudgetbarIcon** icons = gameBudgetbar->getIconsList();
			ChickIcon* cIcon = (ChickIcon*)icons[ICON_CHICK];
			CowIcon* cowIcon = (CowIcon*)icons[ICON_COW];
			SealIcon* sIcon = (SealIcon*)icons[ICON_SEAL];
			WaterIcon* wIcon = (WaterIcon*)icons[ICON_WATER];

			auto checkEat = [&](Animal* animal) {
				bool colliding = false;
				for (size_t j = 0; j < foodList.size(); j++) {
					if (foodList[j] && animal->CollisionDetection(*foodList[j])) {
						colliding = true;
						if (animal->getCanEat()) {
							delete foodList[j];
							foodList.erase(foodList.begin() + j);
							j--;
							grassCount--;
							animal->setCanEat(false);
						}
						return;
					}
				}
				for (int j = 0; j < wIcon->count; j++) {
					if (wIcon->grassList[j] && animal->CollisionDetection(*wIcon->grassList[j])) {
						colliding = true;
						if (animal->getCanEat()) {
							delete wIcon->grassList[j];
							wIcon->grassList[j] = nullptr;
							grassCount--;
							animal->setCanEat(false);
						}
						return;
					}
				}
				if (!colliding) {
					animal->setCanEat(true);
				}
				};

			for (size_t i = 0; i < animalList.size(); i++) if (animalList[i]) checkEat(animalList[i]);
			for (int i = 0; i < cIcon->count; i++) if (cIcon->chickList[i]) checkEat(cIcon->chickList[i]);
			for (int i = 0; i < cowIcon->count; i++) if (cowIcon->CowList[i]) checkEat(cowIcon->CowList[i]);
			for (int i = 0; i < sIcon->count; i++) if (sIcon->sealList[i]) checkEat(sIcon->sealList[i]);

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
					bool actionTaken = false;

					for (size_t i = 0; i < wolves.size(); i++) {
						if (wolves[i] != nullptr && wolves[i]->isClicked(x, y)) {
							actionTaken = true;
							if (wolves[i]->incrementClickCount() >= 5) {
								delete wolves[i];
								wolves.erase(wolves.begin() + i);
								i--;
							}
							break;
						}
					}

					if (!actionTaken) {
						for (size_t i = 0; i < productList.size(); i++) {
							if (productList[i] != nullptr && productList[i]->isClicked(x, y)) {
								if (pWarehouse && pWarehouse->StoreItem(productList[i]->getType())) {
									delete productList[i];
									productList.erase(productList.begin() + i);
									actionTaken = true;
								}
								else {
									printMessage("Warehouse is full!");
								}
								break;
							}
						}
					}

					if (!actionTaken && budget >= 20) {
						point p; p.x = x - 25; p.y = y - 25;
						FoodArea* pNewFood = new FoodArea(this, p, 50, 50, "images\\grass.jpg", 100);
						foodList.push_back(pNewFood);
						budget -= 20;
						grassCount++;
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

void Game::removeWolf(const Wolf* wolf) {
	for (size_t i = 0; i < wolves.size(); i++) {
		if (wolves[i] == wolf) {
			delete wolves[i];
			wolves.erase(wolves.begin() + i);
			return;
		}
	}
}

void Game::saveGame() {
	std::ofstream out("save.txt");
	if (!out.is_open()) return;

	// ---- Header ----
	out << "LEVEL "  << level  << "\t\t// Current level\n";
	out << "BUDGET " << budget << "\t\t// Current player budget\n";
	out << "TIMER "  << remainingTimeSeconds << "\t\t// Remaining time in seconds\n";
	out << "\n";

	BudgetbarIcon** icons = gameBudgetbar->getIconsList();
	ChickIcon* cIcon  = (ChickIcon*)icons[ICON_CHICK];
	CowIcon*   cowIcon = (CowIcon*)icons[ICON_COW];
	SealIcon*  sIcon  = (SealIcon*)icons[ICON_SEAL];
	DogIcon*   dIcon  = (DogIcon*)icons[ICON_DOG];
	WaterIcon* wIcon  = (WaterIcon*)icons[ICON_WATER];

	
	int realChickCount = 0;
	for (int i = 0; i < cIcon->count; i++) if (cIcon->chickList[i]) realChickCount++;
	int realCowCount = 0;
	for (int i = 0; i < cowIcon->count; i++) if (cowIcon->CowList[i]) realCowCount++;
	int realSealCount = 0;
	for (int i = 0; i < sIcon->count; i++) if (sIcon->sealList[i]) realSealCount++;
	int realDogCount = 0;
	for (int i = 0; i < dIcon->count; i++) if (dIcon->dogList[i]) realDogCount++;
	int realWolfCount = (int)wolves.size();
	int realGrassCount = 0;
	for (int i = 0; i < wIcon->count; i++) if (wIcon->grassList[i]) realGrassCount++;

	int totalAnimals = realChickCount + realCowCount + realSealCount + realDogCount;

	
	out << "ANIMALS " << totalAnimals << "\n";
	for (int i = 0; i < cIcon->count; i++)
		if (cIcon->chickList[i])
			out << "CHICK " << cIcon->chickList[i]->getPos().x << " " << cIcon->chickList[i]->getPos().y << "\n";
	for (int i = 0; i < cowIcon->count; i++)
		if (cowIcon->CowList[i])
			out << "COW " << cowIcon->CowList[i]->getPos().x << " " << cowIcon->CowList[i]->getPos().y << "\n";
	for (int i = 0; i < sIcon->count; i++)
		if (sIcon->sealList[i])
			out << "SEAL " << sIcon->sealList[i]->getPos().x << " " << sIcon->sealList[i]->getPos().y << "\n";
	for (int i = 0; i < dIcon->count; i++)
		if (dIcon->dogList[i])
			out << "DOG " << dIcon->dogList[i]->getPos().x << " " << dIcon->dogList[i]->getPos().y << "\n";
	out << "\n";

	
	out << "WOLVES " << realWolfCount << "\n";
	for (size_t i = 0; i < wolves.size(); i++)
		if (wolves[i])
			out << "WOLF " << wolves[i]->getPos().x << " " << wolves[i]->getPos().y << "\n";
	out << "\n";

	
	out << "FOODAREAS " << realGrassCount << "\n";
	for (int i = 0; i < wIcon->count; i++)
		if (wIcon->grassList[i])
			out << "FOOD " << wIcon->grassList[i]->curr_pos.x << " " << wIcon->grassList[i]->curr_pos.y << " 100\n";
	out << "\n";

	
	int eggCount  = pWarehouse ? pWarehouse->GetItemCount(ProductType::EGG)  : 0;
	int milkCount = pWarehouse ? pWarehouse->GetItemCount(ProductType::MILK) : 0;
	int fishCount = pWarehouse ? pWarehouse->GetItemCount(ProductType::FISH) : 0;

	out << "WAREHOUSE\n";
	out << "EGGS " << eggCount  << "\n";
	out << "MILK " << milkCount << "\n";
	out << "FISH " << fishCount << "\n";

	out.close();
	printMessage("Game Saved Successfully!");
}

void Game::loadGame() {
	std::ifstream in("save.txt");
	if (!in.is_open()) {
		printMessage("No save file found!");
		return;
	}

	restartGame();

	BudgetbarIcon** icons = gameBudgetbar->getIconsList();
	ChickIcon* cIcon   = (ChickIcon*)icons[ICON_CHICK];
	CowIcon*   cowIcon = (CowIcon*)icons[ICON_COW];
	SealIcon*  sIcon   = (SealIcon*)icons[ICON_SEAL];
	DogIcon*   dIcon   = (DogIcon*)icons[ICON_DOG];
	WaterIcon* wIcon   = (WaterIcon*)icons[ICON_WATER];

	std::string line;
	while (std::getline(in, line)) {
		if (line.empty()) continue;

		
		size_t commentPos = line.find("//");
		if (commentPos != std::string::npos)
			line = line.substr(0, commentPos);

		std::istringstream ss(line);
		std::string keyword;
		ss >> keyword;
		if (keyword.empty()) continue;

		if (keyword == "LEVEL") {
			ss >> level;
			currentLevel = level;
			goal = GetLevelGoal(level);
		}
		else if (keyword == "BUDGET") {
			ss >> budget;
		}
		else if (keyword == "TIMER") {
			ss >> remainingTimeSeconds;
		}
		else if (keyword == "ANIMALS") {
			
		}
		else if (keyword == "CHICK") {
			int x, y; ss >> x >> y;
			if (cIcon->count < MAX_CREATED_ANIMALS) {
				point p; p.x = x; p.y = y;
				cIcon->chickList[cIcon->count] = new Chick(this, p, 50, 50, cIcon->image_path);
				cIcon->count++;
				animalCount++;
			}
		}
		else if (keyword == "COW") {
			int x, y; ss >> x >> y;
			if (cowIcon->count < MAX_CREATED_ANIMALS) {
				point p; p.x = x; p.y = y;
				cowIcon->CowList[cowIcon->count] = new Cow(this, p, 80, 80, cowIcon->image_path);
				cowIcon->count++;
				animalCount++;
			}
		}
		else if (keyword == "SEAL") {
			int x, y; ss >> x >> y;
			if (sIcon->count < MAX_CREATED_ANIMALS) {
				point p; p.x = x; p.y = y;
				sIcon->sealList[sIcon->count] = new Seal(this, p, 80, 80, sIcon->image_path);
				sIcon->count++;
				animalCount++;
			}
		}
		else if (keyword == "DOG") {
			int x, y; ss >> x >> y;
			if (dIcon->count < MAX_CREATED_ANIMALS) {
				point p; p.x = x; p.y = y;
				dIcon->dogList[dIcon->count] = new Dog(this, p, 60, 60, "images\\dog_sprite.jpg");
				dIcon->count++;
				animalCount++;
			}
		}
		else if (keyword == "WOLVES") {
			
		}
		else if (keyword == "WOLF") {
			int x, y; ss >> x >> y;
			point p; p.x = x; p.y = y;
			drawWolf(p, 70, 70, GetWolfSpeed());
		}
		else if (keyword == "FOODAREAS") {
			
		}
		else if (keyword == "FOOD") {
			int x, y, fc; ss >> x >> y >> fc;
			if (wIcon->count < MAX_CREATED_ANIMALS) {
				point p; p.x = x; p.y = y;
				wIcon->grassList[wIcon->count] = new Grass(this, p, 50, 50, "images\\grass.jpg");
				wIcon->count++;
				grassCount++;
			}
		}
		else if (keyword == "WAREHOUSE") {
			
		}
		else if (keyword == "EGGS") {
			int cnt; ss >> cnt;
			if (pWarehouse && cnt > 0) pWarehouse->StoreItem(ProductType::EGG, cnt);
		}
		else if (keyword == "MILK") {
			int cnt; ss >> cnt;
			if (pWarehouse && cnt > 0) pWarehouse->StoreItem(ProductType::MILK, cnt);
		}
		else if (keyword == "FISH") {
			int cnt; ss >> cnt;
			if (pWarehouse && cnt > 0) pWarehouse->StoreItem(ProductType::FISH, cnt);
		}
	}

	in.close();

	isPaused = true;
	printMessage("Game Loaded Successfully! Click Resume to continue.");
}
