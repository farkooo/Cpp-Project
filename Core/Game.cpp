#include "Game.h"
#include "../Config/GameConfig.h"
#include "Foodarea.h"
#include "Product.h"
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>

Game::Game()
{
	budget = 2000;
	currentLevel = 1;
	gameStartTime = CurrentTime();
	lastWolfSpawnTime = 0;

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


	//4- Create the Warehouse
	point warehousePos;
	warehousePos.x = config.windWidth - 150;
	warehousePos.y = config.windHeight - config.statusBarHeight - 120;
	pWarehouse = new Warehouse(this, warehousePos, 120, 80, 500);

	//5- Create the Plane
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
	//Clear Status bar by drawing a filled rectangle
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
	do {
		position.x = xDist(generator);
		position.y = yDist(generator);
	} while (position.x < 300 && position.y + wolfHeight > config.windHeight - config.statusBarHeight - 300);

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
	currentLevel = 1;
	animalCount = 0;
	grassCount = 0;
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

	pWind->SetPen(GRAY, 10);
	pWind->SetBrush(BLUE);
	pWind->DrawRectangle(0, config.windHeight - config.statusBarHeight - 300, 300, config.windHeight - config.statusBarHeight);
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
	string grassStr = "Grass: " + to_string(grassCount);

	pWind->DrawString(10, config.windHeight - 40, timerStr);
	pWind->DrawString(180, config.windHeight - 40, goalStr);
	pWind->DrawString(330, config.windHeight - 40, levelStr);
	pWind->DrawString(480, config.windHeight - 40, countStr);
	pWind->DrawString(630, config.windHeight - 40, grassStr);
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

	// Use CurrentTime() for high-precision timing (ms)
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

		// 2. Check Lose Condition
		if (remainingTimeSeconds <= 0)
		{
			isPaused = true; // Auto-pause if time is up
			pWind->UpdateBuffer();
			pWind->SetPen(RED, 50);
			pWind->SetFont(40, BOLD, BY_NAME, "Arial");
			pWind->DrawString(config.windWidth / 2 - 150, config.windHeight / 2, "TIME'S UP! YOU LOSE!");
			printMessage("Game Over! Click anywhere to exit...");
			drawTimer();
			pWind->UpdateBuffer(); // Ensure the "You Lose" text actually shows up
			pWind->WaitMouseClick(x, y);
			isExit = true;
			break;
		}

		if (!isPaused)
		{
			// 3. Drawing the environment
			drawField();

			// 4. Update and Draw Food
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

			// 5. Update and Draw Animals
			for (size_t i = 0; i < animalList.size(); i++) {
				if (animalList[i]) {
					animalList[i]->moveStep();
					animalList[i]->draw();

					if (animalList[i]->checkProduction()) {
						point dropPos = animalList[i]->getPos();
						// Note: Ensure Egg class and productList are correctly defined/accessible
						// Product* pNew = new Egg(this, dropPos, 30, 30, "images\\egg.jpg");
						// productList.push_back(pNew);
						budget += 50;
					}
				}
			}

<<<<<<< Updated upstream
			// 6. Update and Draw Wolves
=======
			// Feature #31: Collision Detection - Animals eating grass
			BudgetbarIcon** icons = gameBudgetbar->getIconsList();
			ChickIcon* cIcon = (ChickIcon*)icons[ICON_CHICK];
			CowIcon* cowIcon = (CowIcon*)icons[ICON_COW];
			SealIcon* sIcon = (SealIcon*)icons[ICON_SEAL];
			WaterIcon* wIcon = (WaterIcon*)icons[ICON_WATER];

			auto checkEat = [&](Animal* animal) {
				bool colliding = false;
				// Check foodList (Normal grass)
				for (size_t j = 0; j < foodList.size(); j++) {
					if (foodList[j] && animal->CollisionDetection(*foodList[j])) {
						colliding = true;
						if (animal->getCanEat()) {
							delete foodList[j];
							foodList.erase(foodList.begin() + j);
							j--;
							grassCount--;
							animalCount++; 
							animal->setCanEat(false);
						}
						return; 
					}
				}
				// Check grassList (Water bucket grass)
				for (int j = 0; j < wIcon->count; j++) {
					if (wIcon->grassList[j] && animal->CollisionDetection(*wIcon->grassList[j])) {
						colliding = true;
						if (animal->getCanEat()) {
							delete wIcon->grassList[j];
							wIcon->grassList[j] = nullptr;
							grassCount--;
							animalCount++;
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


>>>>>>> Stashed changes
			for (size_t i = 0; i < wolves.size(); i++) {
				wolves[i]->moveStep();
				wolves[i]->draw();
			}

			// 7. Draw Products
			for (size_t i = 0; i < productList.size(); i++) {
				if (productList[i]) productList[i]->draw();
			}

			// 7.5 Draw Warehouse
			if (pWarehouse) pWarehouse->draw();

			// 7.6 Update and Draw Budgetbar entities
			gameBudgetbar->update();

			// 8. Wolf Spawning Logic
			if (currentGameTime - lastWolfSpawnTime >= 30) {
				generateRandomWolves();
				lastWolfSpawnTime = currentGameTime;
			}
		}
		else {
			// If paused, we still need to draw the field and entities (static)
			drawField();
			for (auto food : foodList) if (food) food->draw();
			for (auto animal : animalList) if (animal) animal->draw();
			for (auto wolf : wolves) if (wolf) wolf->draw();
			for (auto product : productList) if (product) product->draw();
			if (pWarehouse) pWarehouse->draw();
		}

		// 9. UI Updates (Always update UI so buttons work)
		gameToolbar->draw();
		gameBudgetbar->draw();
		gameBudgetbar->update();
		drawStatusBar();
		drawTimer();
		printBudget("BUDGET = $" + to_string(budget));

		// 10. Handle Input
		clicktype click = pWind->GetMouseClick(x, y);
		if (click != NO_CLICK)
		{
			if (y >= 0 && y < config.toolBarHeight) {
				isExit = gameToolbar->handleClick(x, y);
			}
			else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight) {
				isExit = gameBudgetbar->handleClick(x, y);
			}
			else if (y >= 2 * config.toolBarHeight && y < config.windHeight - config.statusBarHeight) {
<<<<<<< Updated upstream
				if (!isPaused && budget >= 20) {
					point p; p.x = x - 25; p.y = y - 25;
					FoodArea* pNewFood = new FoodArea(this, p, 50, 50, "images\\grass.jpg", 100);
					foodList.push_back(pNewFood);
					budget -= 20;
=======
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
						grassCount++;
					}
>>>>>>> Stashed changes
				}
			}
		}

		pWind->UpdateBuffer();
		Sleep(30); // Control frame rate (approx 33 FPS)

	} while (!isExit);
}
void Game::showWarehouse()
{
    int wWidth = 400;
    int wHeight = 300;
    
    window* pWarehouseWind = new window(wWidth, wHeight, config.wx + 50, config.wy + 50);
    pWarehouseWind->ChangeTitle("Warehouse Inventory");

    // 3. Draw the background
    pWarehouseWind->SetPen(LIGHTGRAY, 1);
    pWarehouseWind->DrawRectangle(0, 0, wWidth, wHeight);

    pWarehouseWind->SetPen(BLACK, 50);
    pWarehouseWind->SetFont(24, BOLD, BY_NAME, "Arial");
    pWarehouseWind->DrawString(20, 20, "- Warehouse Inventory -");
    
    pWarehouseWind->SetFont(18, BOLD, BY_NAME, "Arial");
    pWarehouseWind->DrawString(20, 80, "Item 1: 0");
    pWarehouseWind->DrawString(20, 110, "Item 2: 0");

    pWarehouseWind->SetPen(DARKRED, 50);
    pWarehouseWind->DrawString(20, wHeight - 40, "Click anywhere inside to close...");

    int x, y;
    pWarehouseWind->WaitMouseClick(x, y);

    delete pWarehouseWind;
}
<<<<<<< Updated upstream
=======

void Game::addProduct(Product* p) {
	if (p) productList.push_back(p);
}

void Game::saveGame() {
    std::ofstream out("save.txt");
    if (!out.is_open()) return;

    out << level << " // Level\n";
    out << budget << " // Budget\n";
    int m = remainingTimeSeconds / 60;
    int s = remainingTimeSeconds % 60;
    out << m << ":" << (s < 10 ? "0" : "") << s << " // Timer\n\n";

    BudgetbarIcon** icons = gameBudgetbar->getIconsList();
    ChickIcon* cIcon = (ChickIcon*)icons[ICON_CHICK];
    CowIcon* cowIcon = (CowIcon*)icons[ICON_COW];
    SealIcon* sIcon = (SealIcon*)icons[ICON_SEAL];
    WaterIcon* wIcon = (WaterIcon*)icons[ICON_WATER];

    int realChickCount = 0;
    for (int i = 0; i < cIcon->count; i++) if (cIcon->chickList[i]) realChickCount++;
    int realCowCount = 0;
    for (int i = 0; i < cowIcon->count; i++) if (cowIcon->CowList[i]) realCowCount++;
    int realSealCount = 0;
    for (int i = 0; i < sIcon->count; i++) if (sIcon->sealList[i]) realSealCount++;
    int realWolfCount = wolves.size();
    int realGrassCount = 0;
    for (int i = 0; i < wIcon->count; i++) if (wIcon->grassList[i]) realGrassCount++;

    out << realChickCount << " // Number of chickens\n";
    out << realCowCount << " // Number of cows\n";
    out << realSealCount << " // Number of seals\n";
    out << realWolfCount << " // Number of wolves\n";
    out << realGrassCount << " // Number of grass\n\n";

    for (int i = 0; i < cIcon->count; i++) {
        if (cIcon->chickList[i]) {
            out << cIcon->chickList[i]->getPos().x << " // Chick" << i + 1 << "X\n";
            out << cIcon->chickList[i]->getPos().y << " // Chick" << i + 1 << "Y\n";
        }
    }
    out << "\n";
    for (int i = 0; i < cowIcon->count; i++) {
        if (cowIcon->CowList[i]) {
            out << cowIcon->CowList[i]->getPos().x << " // Cow" << i + 1 << "X\n";
            out << cowIcon->CowList[i]->getPos().y << " // Cow" << i + 1 << "Y\n";
        }
    }
    out << "\n";
    for (int i = 0; i < sIcon->count; i++) {
        if (sIcon->sealList[i]) {
            out << sIcon->sealList[i]->getPos().x << " // Seal" << i + 1 << "X\n";
            out << sIcon->sealList[i]->getPos().y << " // Seal" << i + 1 << "Y\n";
        }
    }
    out << "\n";
    for (int i = 0; i < wolves.size(); i++) {
        if (wolves[i]) {
            out << wolves[i]->getPos().x << " // Wolf" << i + 1 << "X\n";
            out << wolves[i]->getPos().y << " // Wolf" << i + 1 << "Y\n";
        }
    }
    out << "\n";
    for (int i = 0; i < wIcon->count; i++) {
        if (wIcon->grassList[i]) {
            out << wIcon->grassList[i]->curr_pos.x << " // Grass" << i + 1 << "X\n";
            out << wIcon->grassList[i]->curr_pos.y << " // Grass" << i + 1 << "Y\n";
        }
    }
    out << "\n";

    int eggCount = pWarehouse ? pWarehouse->GetItemCount(ProductType::EGG) : 0;
    int milkCount = pWarehouse ? pWarehouse->GetItemCount(ProductType::MILK) : 0;
    int fishCount = pWarehouse ? pWarehouse->GetItemCount(ProductType::FISH) : 0;

    int numItems = 0;
    if (eggCount > 0) numItems++;
    if (milkCount > 0) numItems++;
    if (fishCount > 0) numItems++;

    out << numItems << " // Items in the warehouse\n";
    if (eggCount > 0) out << "0 " << eggCount << " // Item type, count (EGG=0)\n";
    if (milkCount > 0) out << "1 " << milkCount << " // Item type, count (MILK=1)\n";
    if (fishCount > 0) out << "2 " << fishCount << " // Item type, count (FISH=2)\n";

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

    std::string line, dummy;
    
    in >> level; std::getline(in, dummy);
    in >> budget; std::getline(in, dummy);

    std::string timerStr;
    in >> timerStr; std::getline(in, dummy);
    size_t pos = timerStr.find(':');
    if (pos != std::string::npos) {
        try {
            int m = std::stoi(timerStr.substr(0, pos));
            int s = std::stoi(timerStr.substr(pos + 1));
            remainingTimeSeconds = m * 60 + s;
        } catch (...) {
            remainingTimeSeconds = 120;
        }
    }

    int numChickens, numCows, numSeals, numWolves, numGrass;
    in >> numChickens; std::getline(in, dummy);
    in >> numCows; std::getline(in, dummy);
    in >> numSeals; std::getline(in, dummy);
    in >> numWolves; std::getline(in, dummy);
    in >> numGrass; std::getline(in, dummy);

    BudgetbarIcon** icons = gameBudgetbar->getIconsList();
    ChickIcon* cIcon = (ChickIcon*)icons[ICON_CHICK];
    CowIcon* cowIcon = (CowIcon*)icons[ICON_COW];
    SealIcon* sIcon = (SealIcon*)icons[ICON_SEAL];
    WaterIcon* wIcon = (WaterIcon*)icons[ICON_WATER];

    for (int i = 0; i < numChickens; i++) {
        int x, y;
        if (!(in >> x)) break; std::getline(in, dummy);
        if (!(in >> y)) break; std::getline(in, dummy);
        point p; p.x = x; p.y = y;
        cIcon->chickList[cIcon->count] = new Chick(this, p, 50, 50, cIcon->image_path);
        cIcon->count++;
        animalCount++;
    }
    for (int i = 0; i < numCows; i++) {
        int x, y;
        if (!(in >> x)) break; std::getline(in, dummy);
        if (!(in >> y)) break; std::getline(in, dummy);
        point p; p.x = x; p.y = y;
        cowIcon->CowList[cowIcon->count] = new Cow(this, p, 80, 80, cowIcon->image_path);
        cowIcon->count++;
        animalCount++;
    }
    for (int i = 0; i < numSeals; i++) {
        int x, y;
        if (!(in >> x)) break; std::getline(in, dummy);
        if (!(in >> y)) break; std::getline(in, dummy);
        point p; p.x = x; p.y = y;
        sIcon->sealList[sIcon->count] = new Seal(this, p, 80, 80, sIcon->image_path);
        sIcon->count++;
        animalCount++;
    }
    for (int i = 0; i < numWolves; i++) {
        int x, y;
        if (!(in >> x)) break; std::getline(in, dummy);
        if (!(in >> y)) break; std::getline(in, dummy);
        point p; p.x = x; p.y = y;
        drawWolf(p, 70, 70, level); 
    }
    for (int i = 0; i < numGrass; i++) {
        int x, y;
        if (!(in >> x)) break; std::getline(in, dummy);
        if (!(in >> y)) break; std::getline(in, dummy);
        point p; p.x = x; p.y = y;
        wIcon->grassList[wIcon->count] = new Grass(this, p, 50, 50, "images\\grass.jpg");
        wIcon->count++;
        grassCount++;
    }

    int numItems = 0;
    if (in >> numItems) {
        std::getline(in, dummy);
        for (int i = 0; i < numItems; i++) {
            int type, count;
            in >> type >> count; std::getline(in, dummy);
            pWarehouse->StoreItem((ProductType)type, count);
        }
    }

    in.close();
    
    currentLevel = level;
    isPaused = true;
    printMessage("Game Loaded Successfully! Click Resume to continue.");
}
>>>>>>> Stashed changes
