	#include "Budgetbar.h"
	#include "../Config/GameConfig.h"
	#include "../Core/Game.h"
	#include <iostream>
	using namespace std;


	BudgetbarIcon::BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
	{
		image_path = img_path;
	}

	void BudgetbarIcon::draw() const
	{
		//draw image of this object
		window* pWind = pGame->getWind();
		pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
	}

	void Budgetbar::update()
	{
		for (int i = 0; i < ANIMAL_COUNT; i++) {
			if (iconsList[i] != nullptr) iconsList[i]->update();
		}
	}

	void Budgetbar::reset()
	{
		for (int i = 0; i < ANIMAL_COUNT; i++) {
			if (iconsList[i] != nullptr) {
				iconsList[i]->reset();
			}
		}
	}

	ChickIcon::ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
	{


		chickList = new Chick * [MAX_CREATED_ANIMALS];
		for (int i = 0; i < MAX_CREATED_ANIMALS; i++) {
			chickList[i] = nullptr;
		}

	}

	void ChickIcon::update() {
		for (int i = 0; i < count; i++) {
			if (chickList[i] != nullptr) {
				chickList[i]->moveStep();
				chickList[i]->draw();
			}
		}
	}

	void ChickIcon::onClick()
	{
		//TO DO: add code for cleanup and game exit here
		/*
		//draw image of this object in the field
		window* pWind = pGame->getWind();
		pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
		*/
		//Chick* new_chick = new Chick(pGame, RefPoint, 30, 30, "images\\Chick.png");
		cout << "Icon Chick Clicked" << endl;
		if (pGame->budget >= 100) {
			pGame->budget = pGame->budget - 100;
			pGame->clearBudget();
			string budget_string = "BUDGET = $" + to_string(pGame->budget);
			pGame->printBudget(budget_string);

			point p;
			// 1. Obtain a seed from a non-deterministic source (if available)
			std::random_device rd1;

			// 2. Seed the Mersenne Twister engine
			// std::mt19937 is a high-quality pseudo-random number generator
			std::mt19937 gen1(rd1());
			std::uniform_int_distribution<int> dist1(range_min_x, range_max_x);
			p.x = dist1(gen1);
			//std::cout << "P.X = " << p.x << endl;
			// 1. Obtain a seed from a non-deterministic source (if available)
			std::random_device rd2;

			// 2. Seed the Mersenne Twister engine
			// std::mt19937 is a high-quality pseudo-random number generator
			std::mt19937 gen2(rd2());
			std::uniform_int_distribution<int> dist2(range_min_y, range_max_y);
			p.y = dist2(gen2);
			//std::cout << "P.Y = " << p.y << endl;
			//p.x = 300;
			//p.y = 300;
			chickList[count] = new Chick(pGame, p, 50, 50, image_path);
			chickList[count]->draw();
			count++;
			pGame->animalCount++;
			pGame->drawStatusBar();
			//window* pWind = pGame->getWind();
			//pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
		}
	}

	void ChickIcon::reset() {
		for (int i = 0; i < count; i++) {
			delete chickList[i]; 
			chickList[i] = nullptr;
		}
		count = 0; 
	}

	CowIcon::CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
	{
		cowList = new Cow * [MAX_CREATED_ANIMALS];
		for (int i = 0; i < MAX_CREATED_ANIMALS; i++) {
			cowList[i] = nullptr;
		}
	}

	void CowIcon::onClick()
	{
		//TO DO: add code for cleanup and game exit here
		/*
		//draw image of this object in the field
		window* pWind = pGame->getWind();
		pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
		*/

		//Chick* new_chick = new Chick(pGame, RefPoint, 30, 30, "images\\Chick.png");
		cout << "Icon Cow Clicked" << endl;
		if (pGame->budget >= 200) {
			pGame->budget = pGame->budget - 200;
			pGame->clearBudget();
			string budget_string = "BUDGET = $" + to_string(pGame->budget);
			pGame->printBudget(budget_string);

			point p;
			// 1. Obtain a seed from a non-deterministic source (if available)
			std::random_device rd1;

			// 2. Seed the Mersenne Twister engine
			// std::mt19937 is a high-quality pseudo-random number generator
			std::mt19937 gen1(rd1());
			std::uniform_int_distribution<int> dist1(range_min_x, range_max_x);
			p.x = dist1(gen1);
			//std::cout << "P.X = " << p.x << endl;
			// 1. Obtain a seed from a non-deterministic source (if available)
			std::random_device rd2;

			// 2. Seed the Mersenne Twister engine
			// std::mt19937 is a high-quality pseudo-random number generator
			std::mt19937 gen2(rd2());
			std::uniform_int_distribution<int> dist2(range_min_y, range_max_y);
			p.y = dist2(gen2);
			//std::cout << "P.Y = " << p.y << endl;
			//p.x = 300;
			//p.y = 300;
			cowList[count] = new Cow(pGame, p, 80, 80, image_path);
			cowList[count]->draw();
			count++;
			pGame->animalCount++;
			pGame->drawStatusBar();
			//window* pWind = pGame->getWind();
			//pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
		}
	}
	void CowIcon::update() {
		for (int i = 0; i < count; i++) {
			if (cowList[i] != nullptr) {
				cowList[i]->moveStep();
				cowList[i]->draw();
			}
		}
	}

	void CowIcon::reset() {
		for (int i = 0; i < count; i++) {
			delete cowList[i]; 
			cowList[i] = nullptr;
		}
		count = 0; 
	}

	SealIcon::SealIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
	{
		sealList = new Seal * [MAX_CREATED_ANIMALS];
		for (int i = 0; i < MAX_CREATED_ANIMALS; i++) {
			sealList[i] = nullptr;
		}
	}

	void SealIcon::onClick()
	{
		//TO DO: add code for cleanup and game exit here
		/*
		//draw image of this object in the field
		window* pWind = pGame->getWind();
		pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
		*/

		//Chick* new_chick = new Chick(pGame, RefPoint, 30, 30, "images\\Chick.png");
		cout << "Icon Seal Clicked" << endl;
		if (pGame->budget >= 300) {
			pGame->budget = pGame->budget - 300;
			pGame->clearBudget();
			string budget_string = "BUDGET = $" + to_string(pGame->budget);
			pGame->printBudget(budget_string);

			point p;
			// 1. Obtain a seed from a non-deterministic source (if available)
			std::random_device rd1;

			// 2. Seed the Mersenne Twister engine
			// std::mt19937 is a high-quality pseudo-random number generator
			std::mt19937 gen1(rd1());
			std::uniform_int_distribution<int> dist1(range_min_x, range_max_x);
			p.x = dist1(gen1);
			//std::cout << "P.X = " << p.x << endl;
			// 1. Obtain a seed from a non-deterministic source (if available)
			std::random_device rd2;

			// 2. Seed the Mersenne Twister engine
			// std::mt19937 is a high-quality pseudo-random number generator
			std::mt19937 gen2(rd2());
			std::uniform_int_distribution<int> dist2(range_min_y, range_max_y);
			p.y = dist2(gen2);
			//std::cout << "P.Y = " << p.y << endl;
			//p.x = 300;
			//p.y = 300;
			sealList[count] = new Seal(pGame, p, 100, 100, image_path);
			sealList[count]->draw();
			count++;
			pGame->animalCount++;
			pGame->drawStatusBar();
			//window* pWind = pGame->getWind();
			//pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
		}
	}

	void SealIcon::update() {
		for (int i = 0; i < count; i++) {
			if (sealList[i] != nullptr) {
				sealList[i]->moveStep();
				sealList[i]->draw();
			}
		}
	}

	void SealIcon::reset() {
		for (int i = 0; i < count; i++) {
			delete sealList[i]; 
			sealList[i] = nullptr;
		}
		count = 0; 
	}

	WaterIcon::WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
	{
		grassList = new Grass * [MAX_CREATED_ANIMALS];
		for (int i = 0; i < MAX_CREATED_ANIMALS; i++) {
			grassList[i] = nullptr;
		}
	}


	void WaterIcon::onClick()
	{
		//TO DO: add code for cleanup and game exit here
		/*
		//draw image of this object in the field
		window* pWind = pGame->getWind();
		pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
		*/
		//Chick* new_chick = new Chick(pGame, RefPoint, 30, 30, "images\\Chick.png");
		cout << "Icon Water Clicked" << endl;
		if (pGame->budget >= 100) {
			pGame->budget = pGame->budget - 100;
			pGame->clearBudget();
			string budget_string = "BUDGET = $" + to_string(pGame->budget);
			pGame->printBudget(budget_string);

			point p;
			// 1. Obtain a seed from a non-deterministic source (if available)
			std::random_device rd1;

			// 2. Seed the Mersenne Twister engine
			// std::mt19937 is a high-quality pseudo-random number generator
			std::mt19937 gen1(rd1());
			std::uniform_int_distribution<int> dist1(range_min_x, range_max_x);
			p.x = dist1(gen1);
			//std::cout << "P.X = " << p.x << endl;
			// 1. Obtain a seed from a non-deterministic source (if available)
			std::random_device rd2;

			// 2. Seed the Mersenne Twister engine
			// std::mt19937 is a high-quality pseudo-random number generator
			std::mt19937 gen2(rd2());
			std::uniform_int_distribution<int> dist2(range_min_y, range_max_y);
			p.y = dist2(gen2);
			//std::cout << "P.Y = " << p.y << endl;
			//p.x = 300;
			//p.y = 300;
			grassList[count] = new Grass(pGame, p, 50, 50, "images\\grass.jpg");
			grassList[count]->draw();
			count++;
			//window* pWind = pGame->getWind();
			//pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
		}
	}


	void WaterIcon::update() {
		for (int i = 0; i < count; i++) {
			if (grassList[i] != nullptr) {
				grassList[i]->moveStep();
				grassList[i]->draw();
			}
		}
	}

	void WaterIcon::reset() {
		for (int i = 0; i < count; i++) {
			delete grassList[i]; 
			grassList[i] = nullptr;
		}
		count = 0; 
	}

	Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
	{
		//First prepare List of images for each icon
		//To control the order of these images in the menu, reoder them in enum ICONS above	
		iconsImages[ICON_CHICK] = "images\\chick.jpg";
		iconsImages[ICON_COW] = "images\\cow.jpg";
		iconsImages[ICON_SEAL] = "images\\seal.jpg";
		iconsImages[ICON_WATER] = "images\\waterbucket.jpg";

		point p;
		p.x = 0;
		p.y = config.toolBarHeight;

		iconsList = new BudgetbarIcon * [ANIMAL_COUNT];
		for (int i = 0; i < ANIMAL_COUNT; i++) iconsList[i] = nullptr;
		//For each icon in the tool bar create an object 
		iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);
		p.x += config.iconWidth;
		iconsList[ICON_COW] = new CowIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_COW]);
		p.x += config.iconWidth;
		iconsList[ICON_SEAL] = new SealIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_SEAL]);
		p.x += config.iconWidth;
		iconsList[ICON_WATER] = new WaterIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_WATER]);
		p.x += config.iconWidth;
		//p.x += config.iconWidth;
		//iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);
	}

	Budgetbar::~Budgetbar()
	{
		for (int i = 0; i < ANIMAL_COUNT; i++)
			delete iconsList[i];
		delete[] iconsList;
	}

	void Budgetbar::draw() const
	{
		for (int i = 0; i < ANIMAL_COUNT; i++) {
			if (iconsList[i] != nullptr) iconsList[i]->draw();
		}
		window* pWind = pGame->getWind();
		pWind->SetPen(BLACK, 3);
		pWind->DrawLine(0, 2 * config.toolBarHeight, pWind->GetWidth(), 2 * config.toolBarHeight);
	}

	bool Budgetbar::handleClick(int x, int y)
	{
		if (x < 0 || x >= ANIMAL_COUNT * config.iconWidth)	//click outside toolbar boundaries
			return false;


		if (y < config.toolBarHeight || y >= 2 * config.toolBarHeight)
			return false;


		//Check whick icon was clicked
		//==> This assumes that menu icons are lined up horizontally <==
		//Divide x co-ord of the point clicked by the icon width (int division)
		//if division result is 0 ==> first icon is clicked, if 1 ==> 2nd icon and so on

		int clickedIconIndex = (x / config.iconWidth);
		iconsList[clickedIconIndex]->onClick();	//execute onClick action of clicled icon

		//if (clickedIconIndex == ICON_EXIT) return true;

		return false;

	}