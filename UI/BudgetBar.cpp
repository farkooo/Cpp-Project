#include "Budgetbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include "../product.h"
#include <iostream>
#include <random>
#include <vector> 

BudgetbarIcon::BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
    : Drawable(r_pGame, r_point, r_width, r_height)
{
    image_path = img_path;
}

void BudgetbarIcon::draw() const
{
    window* pWind = pGame->getWind();
    pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

ChickIcon::ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
    : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
    chickList = new Chick * [MAX_CREATED_ANIMALS];
    for (int i = 0; i < MAX_CREATED_ANIMALS; i++) {
        chickList[i] = nullptr;
        lastProdTime[i] = 999999;
    }
}

void ChickIcon::onClick()
{
    if (pGame->isGamePaused()) return;
    if (pGame->budget >= 100 && count < MAX_CREATED_ANIMALS) {
        pGame->budget -= 100;

        point p;
        std::random_device rd;
        std::mt19937 gen(rd());

        int chickWidth = 50;
        int chickHeight = 50;
        int safe_max_x = config.windWidth - chickWidth - 10;
        int safe_max_y = config.windHeight - config.statusBarHeight - chickHeight - 10;

        std::uniform_int_distribution<int> distX(range_min_x, safe_max_x);
        std::uniform_int_distribution<int> distY(range_min_y, safe_max_y);

        p.x = distX(gen);
        p.y = distY(gen);

        chickList[count] = new Chick(pGame, p, chickWidth, chickHeight, image_path);
        chickList[count]->draw();
        count++;
        pGame->animalCount++;
    }
}

void ChickIcon::update() {
    for (int i = 0; i < count; i++) {
        if (chickList[i]) {
            if (!pGame->isGamePaused()) {
                chickList[i]->moveStep();
            }

            bool eaten = false;
            for (const Wolf* wolf : pGame->getWolves()) {
                if (wolf != nullptr && chickList[i]->isColliding(wolf)) {
                    delete chickList[i];
                    chickList[i] = nullptr;
                    pGame->animalCount--;
                    eaten = true;
                    break;
                }
            }

            if (eaten) continue;

            chickList[i]->draw();

            if (chickList[i]->checkProduction()) {
                unsigned long currTime = pGame->getGameTime();
                if (currTime != lastProdTime[i]) {
                    point dropPos = chickList[i]->getPos();
                    Product* egg = new Egg(pGame, dropPos, 50, 50, "images\\egg.jpg");
                    pGame->addProduct(egg);
                    lastProdTime[i] = currTime;
                }
            }
        }
    }
}

void ChickIcon::draw() const {
    BudgetbarIcon::draw();
    window* pWind = pGame->getWind();
    pWind->SetPen(BLACK, 1);
    pWind->SetBrush(WHITE);
    pWind->DrawRectangle(RefPoint.x + 5, RefPoint.y + height - 22, RefPoint.x + 45, RefPoint.y + height - 2);
    pWind->SetPen(RED);
    pWind->SetFont(16, BOLD, BY_NAME, "Arial");
    pWind->DrawString(RefPoint.x + 8, RefPoint.y + height - 20, "$100");
}

void ChickIcon::reset() {
    for (int i = 0; i < count; i++) {
        delete chickList[i];
        chickList[i] = nullptr;
        lastProdTime[i] = 999999;
    }
    count = 0;
}

CowIcon::CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
    : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
    CowList = new Cow * [MAX_CREATED_ANIMALS];
    for (int i = 0; i < MAX_CREATED_ANIMALS; i++) {
        CowList[i] = nullptr;
        lastProdTime[i] = 999999;
    }
}

void CowIcon::onClick() {
    if (pGame->isGamePaused()) return;
    if (pGame->budget >= 200 && count < MAX_CREATED_ANIMALS) {
        pGame->budget -= 200;

        point p;
        std::random_device rd;
        std::mt19937 gen(rd());

        int cowWidth = 80;
        int cowHeight = 80;
        int safe_max_x = config.windWidth - cowWidth - 10;
        int safe_max_y = config.windHeight - config.statusBarHeight - cowHeight - 10;

        std::uniform_int_distribution<int> distX(range_min_x, safe_max_x);
        std::uniform_int_distribution<int> distY(range_min_y, safe_max_y);

        p.x = distX(gen);
        p.y = distY(gen);

        CowList[count] = new Cow(pGame, p, cowWidth, cowHeight, image_path);
        CowList[count]->draw();
        count++;
        pGame->animalCount++;
    }
}

void CowIcon::update() {
    for (int i = 0; i < count; i++) {
        if (CowList[i]) {
            if (!pGame->isGamePaused()) {
                CowList[i]->moveStep();
            }

            bool eaten = false;
            for (const Wolf* wolf : pGame->getWolves()) {
                if (wolf != nullptr && CowList[i]->isColliding(wolf)) {
                    delete CowList[i];
                    CowList[i] = nullptr;
                    pGame->animalCount--;
                    eaten = true;
                    break;
                }
            }

            if (eaten) continue;

            CowList[i]->draw();

            if (CowList[i]->checkProduction()) {
                unsigned long currTime = pGame->getGameTime();
                if (currTime != lastProdTime[i]) {
                    point dropPos = CowList[i]->getPos();
                    Product* milk = new Milk(pGame, dropPos, 50, 50, "images\\milk.jpg");
                    pGame->addProduct(milk);
                    lastProdTime[i] = currTime;
                }
            }
        }
    }
}

void CowIcon::draw() const {
    BudgetbarIcon::draw();
    window* pWind = pGame->getWind();
    pWind->SetPen(BLACK, 1);
    pWind->SetBrush(WHITE);
    pWind->DrawRectangle(RefPoint.x + 5, RefPoint.y + height - 22, RefPoint.x + 45, RefPoint.y + height - 2);
    pWind->SetPen(RED);
    pWind->SetFont(16, BOLD, BY_NAME, "Arial");
    pWind->DrawString(RefPoint.x + 8, RefPoint.y + height - 20, "$200");
}

void CowIcon::reset() {
    for (int i = 0; i < count; i++) {
        delete CowList[i];
        CowList[i] = nullptr;
        lastProdTime[i] = 999999;
    }
    count = 0;
}

SealIcon::SealIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
    : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
    sealList = new Seal * [MAX_CREATED_ANIMALS];
    for (int i = 0; i < MAX_CREATED_ANIMALS; i++) {
        sealList[i] = nullptr;
        lastProdTime[i] = 999999;
    }
}

void SealIcon::onClick() {
    if (pGame->isGamePaused()) return;
    if (pGame->budget >= 300 && count < MAX_CREATED_ANIMALS) {
        pGame->budget -= 300;

        point p;
        std::random_device rd;
        std::mt19937 gen(rd());

        int sealWidth = 100;
        int sealHeight = 100;
        int safe_max_x = config.windWidth - sealWidth - 10;
        int safe_max_y = config.windHeight - config.statusBarHeight - sealHeight - 10;

        std::uniform_int_distribution<int> distX(range_min_x, safe_max_x);
        std::uniform_int_distribution<int> distY(range_min_y, safe_max_y);

        p.x = distX(gen);
        p.y = distY(gen);

        sealList[count] = new Seal(pGame, p, sealWidth, sealHeight, image_path);
        sealList[count]->draw();
        count++;
        pGame->animalCount++;
    }
}

void SealIcon::update() {
    for (int i = 0; i < count; i++) {
        if (sealList[i]) {
            if (!pGame->isGamePaused()) {
                sealList[i]->moveStep();
            }

            bool eaten = false;
            for (const Wolf* wolf : pGame->getWolves()) {
                if (wolf != nullptr && sealList[i]->isColliding(wolf)) {
                    delete sealList[i];
                    sealList[i] = nullptr;
                    pGame->animalCount--;
                    eaten = true;
                    break;
                }
            }

            if (eaten) continue;

            sealList[i]->draw();

            if (sealList[i]->checkProduction()) {
                unsigned long currTime = pGame->getGameTime();
                if (currTime != lastProdTime[i]) {
                    point dropPos = sealList[i]->getPos();
                    Product* fish = new Fish(pGame, dropPos, 50, 50, "images\\fish1.jpg");
                    pGame->addProduct(fish);
                    lastProdTime[i] = currTime;
                }
            }
        }
    }
}

void SealIcon::draw() const {
    BudgetbarIcon::draw();
    window* pWind = pGame->getWind();
    pWind->SetPen(BLACK, 1);
    pWind->SetBrush(WHITE);
    pWind->DrawRectangle(RefPoint.x + 5, RefPoint.y + height - 22, RefPoint.x + 45, RefPoint.y + height - 2);
    pWind->SetPen(RED);
    pWind->SetFont(16, BOLD, BY_NAME, "Arial");
    pWind->DrawString(RefPoint.x + 8, RefPoint.y + height - 20, "$300");
}

void SealIcon::reset() {
    for (int i = 0; i < count; i++) {
        delete sealList[i];
        sealList[i] = nullptr;
        lastProdTime[i] = 999999;
    }
    count = 0;
}

WaterIcon::WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path)
    : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
    grassList = new Grass * [MAX_CREATED_ANIMALS];
    for (int i = 0; i < MAX_CREATED_ANIMALS; i++) grassList[i] = nullptr;
}

void WaterIcon::onClick() {
    if (pGame->isGamePaused()) return;
    if (pGame->budget >= 100 && count < MAX_CREATED_ANIMALS) {
        pGame->budget -= 100;

        point p;
        std::random_device rd;
        std::mt19937 gen(rd());

        int grassWidth = 50;
        int grassHeight = 50;
        int safe_max_x = config.windWidth - grassWidth - 10;
        int safe_max_y = config.windHeight - config.statusBarHeight - grassHeight - 10;

        std::uniform_int_distribution<int> distX(range_min_x, safe_max_x);
        std::uniform_int_distribution<int> distY(range_min_y, safe_max_y);

        p.x = distX(gen);
        p.y = distY(gen);

        grassList[count] = new Grass(pGame, p, grassWidth, grassHeight, "images\\grass.jpg");
        grassList[count]->draw();
        count++;
    }
}

void WaterIcon::update() {
    for (int i = 0; i < count; i++) {
        if (grassList[i]) {
            if (!pGame->isGamePaused()) {
                grassList[i]->moveStep();
            }
            grassList[i]->draw();
        }
    }
}

void WaterIcon::draw() const {
    BudgetbarIcon::draw();
    window* pWind = pGame->getWind();
    pWind->SetPen(BLACK, 1);
    pWind->SetBrush(WHITE);
    pWind->DrawRectangle(RefPoint.x + 5, RefPoint.y + height - 22, RefPoint.x + 45, RefPoint.y + height - 2);
    pWind->SetPen(RED);
    pWind->SetFont(16, BOLD, BY_NAME, "Arial");
    pWind->DrawString(RefPoint.x + 8, RefPoint.y + height - 20, "$100");
}

void WaterIcon::reset() {
    for (int i = 0; i < count; i++) { delete grassList[i]; grassList[i] = nullptr; }
    count = 0;
}

Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height)
    : Drawable(r_pGame, r_point, r_width, r_height)
{
    iconsImages[ICON_CHICK] = "images\\chick.jpg";
    iconsImages[ICON_COW] = "images\\cow.jpg";
    iconsImages[ICON_SEAL] = "images\\seal.jpg";
    iconsImages[ICON_WATER] = "images\\waterbucket.jpg";

    iconsList = new BudgetbarIcon * [ANIMAL_COUNT];
    point p = { 0, config.toolBarHeight };

    iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);
    p.x += config.iconWidth;
    iconsList[ICON_COW] = new CowIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_COW]);
    p.x += config.iconWidth;
    iconsList[ICON_SEAL] = new SealIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_SEAL]);
    p.x += config.iconWidth;
    iconsList[ICON_WATER] = new WaterIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_WATER]);
}

Budgetbar::~Budgetbar() {
    for (int i = 0; i < ANIMAL_COUNT; i++) delete iconsList[i];
    delete[] iconsList;
}

void Budgetbar::draw() const {
    for (int i = 0; i < ANIMAL_COUNT; i++) {
        if (iconsList[i]) iconsList[i]->draw();
    }

    window* pWind = pGame->getWind();
    int textStartX = (ANIMAL_COUNT * config.iconWidth) + 20;
    int textStartY = config.toolBarHeight + 10;

    pWind->SetPen(DARKBLUE, 50);
    pWind->SetFont(18, BOLD, BY_NAME, "Arial");
    pWind->DrawString(textStartX, textStartY, "Animals Buying: Chick ($100) | Cow ($200) | Seal ($300)");
    pWind->DrawString(textStartX, textStartY + 25, "Water Buying: Water Bucket ($100)");

    pWind->SetPen(BLACK, 3);
    pWind->DrawLine(0, 2 * config.toolBarHeight, pWind->GetWidth(), 2 * config.toolBarHeight);
}

bool Budgetbar::handleClick(int x, int y) {
    if (pGame->isGamePaused()) return false;
    if (x < 0 || x >= ANIMAL_COUNT * config.iconWidth) return false;
    if (y < config.toolBarHeight || y >= 2 * config.toolBarHeight) return false;

    int clickedIconIndex = (x / config.iconWidth);
    iconsList[clickedIconIndex]->onClick();
    return false;
}

void Budgetbar::update() {
    for (int i = 0; i < ANIMAL_COUNT; i++) {
        if (iconsList[i]) iconsList[i]->update();
    }
}

void Budgetbar::reset() {
    for (int i = 0; i < ANIMAL_COUNT; i++) {
        if (iconsList[i]) iconsList[i]->reset();
    }
}