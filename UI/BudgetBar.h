#pragma once
#include "../Core/Drawable.h"
#include "../Entities/Animal.h"
#include "../Config/GameConfig.h"
#include <random>
#include <string>

const int range_min_x = 50;
const int range_max_x = config.windWidth - 50;
const int range_min_y = (config.toolBarHeight * 2) + 50;
const int range_max_y = config.windHeight - config.statusBarHeight - 50;
const int MAX_CREATED_ANIMALS = 15;

class Game;
class Product;

class BudgetbarIcon : public Drawable
{
public:
    std::string image_path;
    BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
    virtual void draw() const override;
    virtual void onClick() = 0;
    virtual void update() {}
    virtual void reset() = 0;
};

class ChickIcon : public BudgetbarIcon
{
public:
    Chick** chickList;
    unsigned long lastProdTime[MAX_CREATED_ANIMALS];
    int count = 0;
    ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
    virtual void onClick() override;
    void update() override;
    virtual void draw() const override;
    virtual void reset() override;
    virtual ~ChickIcon();
};

class CowIcon : public BudgetbarIcon
{
public:
    Cow** CowList;
    unsigned long lastProdTime[MAX_CREATED_ANIMALS];
    int count = 0;
    CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
    virtual void onClick() override;
    void update() override;
    virtual void draw() const override;
    virtual void reset() override;
    virtual ~CowIcon();
};

class SealIcon : public BudgetbarIcon
{
public:
    Seal** sealList;
    unsigned long lastProdTime[MAX_CREATED_ANIMALS];
    int count = 0;
    SealIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
    virtual void onClick() override;
    void update() override;
    virtual void draw() const override;
    virtual void reset() override;
    virtual ~SealIcon();
};

class DogIcon : public BudgetbarIcon
{
public:
    Dog** dogList;
    int count = 0;
    DogIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
    virtual void onClick() override;
    void update() override;
    virtual void draw() const override;
    virtual void reset() override;
    virtual ~DogIcon();
};

class WaterIcon : public BudgetbarIcon
{
public:
    Grass** grassList;
    int count = 0;
    WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path);
    virtual void onClick() override;
    void update() override;
    virtual void draw() const override;
    virtual void reset() override;
    virtual ~WaterIcon();
};

enum ANIMAL_ICONS
{
    ICON_CHICK,
    ICON_COW,
    ICON_SEAL,
    ICON_DOG,
    ICON_WATER,
    ANIMAL_COUNT
};

class Budgetbar : public Drawable
{
private:
    BudgetbarIcon** iconsList;
    std::string iconsImages[ANIMAL_COUNT];
public:
    Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height);
    ~Budgetbar();
    void draw() const override;
    bool handleClick(int x, int y);
    void update();
    void reset();
    BudgetbarIcon** getIconsList() const { return iconsList; }
};
