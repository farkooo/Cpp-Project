#include "Warehouse.h"
#include "Core/Game.h"

Warehouse::Warehouse(Game* r_pGame, point ref, int r_width, int r_height, int cap)
    : GameObject(r_pGame, ref, r_width, r_height, DARKGRAY, BLACK),
    capacity(cap),
    currentItemCount(0)
{
    storedItems[ProductType::EGG] = 0;
    storedItems[ProductType::MILK] = 0;
}

Warehouse::~Warehouse() {}

void Warehouse::draw() const
{
    window* pWind = pGame->getWind();
    pWind->SetPen(borderColor, 2);
    pWind->SetBrush(fillColor);
    pWind->DrawRectangle(RefPoint.x, RefPoint.y, RefPoint.x + width, RefPoint.y + height);

    pWind->SetBrush(DARKRED);
    int roofHeight = height / 3;
    int px[3] = { RefPoint.x - 10, RefPoint.x + width / 2, RefPoint.x + width + 10 };
    int py[3] = { RefPoint.y, RefPoint.y - roofHeight, RefPoint.y };
    pWind->DrawPolygon(px, py, 3);

    pWind->SetBrush(SADDLEBROWN);
    int doorWidth = width / 3;
    int doorHeight = (int)(height * 0.6);
    pWind->DrawRectangle(RefPoint.x + (width - doorWidth) / 2, RefPoint.y + height - doorHeight,
        RefPoint.x + (width + doorWidth) / 2, RefPoint.y + height);

    // Glass Windows
    pWind->SetBrush(LIGHTBLUE);
    int winSize = width / 5;
    int winY = RefPoint.y + height / 5;

    // Left window
    int leftWinX = RefPoint.x + (width / 6) - (winSize / 2);
    pWind->DrawRectangle(leftWinX, winY, leftWinX + winSize, winY + winSize);

    // Right window
    int rightWinX = RefPoint.x + (5 * width / 6) - (winSize / 2);
    pWind->DrawRectangle(rightWinX, winY, rightWinX + winSize, winY + winSize);

    // Capacity Bar
    int barWidth = width;
    int barHeight = 12;
    int barX = RefPoint.x;
    int barY = RefPoint.y + height + 10;
    pWind->SetPen(BLACK, 1);
    pWind->SetBrush(color(200, 200, 200));
    pWind->DrawRectangle(barX, barY, barX + barWidth, barY + barHeight);

    if (currentItemCount > 0) {
        float ratio = (float)currentItemCount / capacity;
        pWind->SetBrush(ratio > 0.9f ? RED : GREEN);
        pWind->DrawRectangle(barX, barY, barX + (int)(barWidth * ratio), barY + barHeight);
    }
}

bool Warehouse::StoreItem(ProductType item, int count) {
    if (currentItemCount + count > capacity) return false; 
    storedItems[item] += count;
    currentItemCount += count;
    return true;
}

bool Warehouse::RemoveItem(ProductType item, int count) {
    if (storedItems[item] < count) return false;
    storedItems[item] -= count;
    currentItemCount -= count;
    return true;
}

int Warehouse::GetItemCount(ProductType item) const { return storedItems.at(item); }
int Warehouse::GetTotalItems() const { return currentItemCount; }
int Warehouse::GetCapacity() const { return capacity; }

int Warehouse::GetProductPrice(ProductType item) {
    switch (item) {
    case ProductType::EGG:
        return 20;
    case ProductType::MILK:
        return 40;
    default:
        return 0;
    }
}

void Warehouse::Reset() {
    currentItemCount = 0;
    for (auto& pair : storedItems) pair.second = 0;
}