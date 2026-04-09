#pragma once
#include <map>

class GameObject;
class Drawable;

enum class ProductType {
    EGG,
    MILK
};

class Warehouse 
{
private:
    int capacity;
    int currentItemCount;
    std::map<ProductType, int> storedItems;

public:
    Warehouse(int cap = 100);
    virtual ~Warehouse();

    bool StoreItem(ProductType item, int count = 1);
    bool RemoveItem(ProductType item, int count = 1);

    int GetItemCount(ProductType item) const;
    int GetTotalItems() const;
    int GetCapacity() const;

};
