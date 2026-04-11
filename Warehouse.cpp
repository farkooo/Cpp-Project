#include "Warehouse.h"

Warehouse::Warehouse(int cap) : capacity(cap), currentItemCount(0)
{
    storedItems[ProductType::EGG] = 0;
    storedItems[ProductType::MILK] = 0;
}

Warehouse::~Warehouse()
{
}

bool Warehouse::StoreItem(ProductType item, int count)
{
    if (currentItemCount + count > capacity)
        return false; // Not enough capacity

    storedItems[item] += count;
    currentItemCount += count;
    return true;
}

bool Warehouse::RemoveItem(ProductType item, int count)
{
    if (storedItems[item] < count)
        return false; // Not enough items of this type

    storedItems[item] -= count;
    currentItemCount -= count;
    return true;
}

int Warehouse::GetItemCount(ProductType item) const
{
    auto it = storedItems.find(item);
    if (it != storedItems.end())
        return it->second;
    return 0;
}

int Warehouse::GetTotalItems() const
{
    return currentItemCount;
}

int Warehouse::GetCapacity() const
{
    return capacity;
}
