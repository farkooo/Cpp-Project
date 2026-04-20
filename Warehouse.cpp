#include "Warehouse.h"
#include "Core/Game.h"

Warehouse::Warehouse(Game* r_pGame, point ref, int r_width, int r_height, int cap) 
    : GameObject(r_pGame, ref, r_width, r_height, DARKGRAY, BLACK), capacity(cap), currentItemCount(0)
{
    storedItems[ProductType::EGG] = 0;
    storedItems[ProductType::MILK] = 0;
}

Warehouse::~Warehouse()
{
}

void Warehouse::draw() const
{
    window* pWind = pGame->getWind();
    
    // Draw building body (Classic Warehouse look)
    pWind->SetPen(borderColor, 2);
    pWind->SetBrush(fillColor);
    pWind->DrawRectangle(RefPoint.x, RefPoint.y, RefPoint.x + width, RefPoint.y + height);

    // Draw a peaked roof
    pWind->SetBrush(DARKRED);
    int roofHeight = height / 3;
    int px[3] = { RefPoint.x - 10, RefPoint.x + width / 2, RefPoint.x + width + 10 };
    int py[3] = { RefPoint.y, RefPoint.y - roofHeight, RefPoint.y };
    pWind->DrawPolygon(px, py, 3);

    // Draw decorative windows
    pWind->SetBrush(LIGHTBLUE);
    pWind->DrawRectangle(RefPoint.x + 10, RefPoint.y + 20, RefPoint.x + 30, RefPoint.y + 40);
    pWind->DrawRectangle(RefPoint.x + width - 30, RefPoint.y + 20, RefPoint.x + width - 10, RefPoint.y + 40);

    // Draw main door
    pWind->SetBrush(SADDLEBROWN);
    int doorWidth = width / 3;
    int doorHeight = (int)(height * 0.6);
    pWind->DrawRectangle(RefPoint.x + (width - doorWidth) / 2, RefPoint.y + height - doorHeight, 
                         RefPoint.x + (width + doorWidth) / 2, RefPoint.y + height);

    // Draw Capacity/Fullness Bar
    int barWidth = width;
    int barHeight = 12;
    int barX = RefPoint.x;
    int barY = RefPoint.y + height + 10;
    
    // Bar background (Empty part)
    pWind->SetPen(BLACK, 1);
    pWind->SetBrush(color(200, 200, 200)); // Light Grey
    pWind->DrawRectangle(barX, barY, barX + barWidth, barY + barHeight);
    
    // Filled part
    if (currentItemCount > 0) {
        float ratio = (float)currentItemCount / capacity;
        if (ratio > 1.0f) ratio = 1.0f;
        
        // Color changes based on fullness
        color barColor = GREEN;
        if (ratio > 0.9f) barColor = RED;
        else if (ratio > 0.7f) barColor = ORANGE;
        else if (ratio > 0.5f) barColor = YELLOW;
        
        pWind->SetBrush(barColor);
        pWind->DrawRectangle(barX, barY, barX + (int)(barWidth * ratio), barY + barHeight);
    }


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

void Warehouse::Reset()
{
    currentItemCount = 0;
    for (auto& pair : storedItems)
    {
        pair.second = 0;
    }
}
