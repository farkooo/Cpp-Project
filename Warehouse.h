#pragma once
#include <map>
#include "Core/GameObject.h"

enum class ProductType {
    EGG,
    MILK
};

class Warehouse : public GameObject
{
private:
    int capacity;
    int currentItemCount;
    std::map<ProductType, int> storedItems;

public:
    Warehouse(Game* r_pGame, point ref, int r_width, int r_height, int cap = 100);
    virtual ~Warehouse();

    virtual void draw() const override;

    bool StoreItem(ProductType item, int count = 1);
    bool RemoveItem(ProductType item, int count = 1);

    int GetItemCount(ProductType item) const;
    int GetTotalItems() const;
    int GetCapacity() const;

    void Reset();

};
