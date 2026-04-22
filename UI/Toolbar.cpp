#include "Toolbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"

ToolbarIcon::ToolbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
}

void ToolbarIcon::draw() const
{
	//draw image of this object
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

RestartIcon::RestartIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{}

void RestartIcon::onClick()
{
	pGame->restartGame();
}

ExitIcon::ExitIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{}

void ExitIcon::onClick()
{
	//TO DO: add code for cleanup and game exit here
}

static void drawButton(window* pWind, int x, int y, int w, int h, color btnColor, string label)
{
	pWind->SetBrush(btnColor);
	pWind->SetPen(WHITE, 2);
	pWind->DrawRectangle(x, y, x + w, y + h);
	pWind->SetPen(WHITE, 1);
	pWind->SetFont(12, BOLD, BY_NAME, "Arial");
	pWind->DrawString(x + 5, y + h / 2 - 7, label);
}

PauseIcon::PauseIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {}

void PauseIcon::draw() const {
	drawButton(pGame->getWind(), RefPoint.x, RefPoint.y, width, height, DARKGREY, "Pause");
}
void PauseIcon::onClick() {
	pGame->setPaused(true);
}

ResumeIcon::ResumeIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {}

void ResumeIcon::draw() const {
	drawButton(pGame->getWind(), RefPoint.x, RefPoint.y, width, height, DARKGREEN, "Resume");
}
void ResumeIcon::onClick() {
	if (pGame->remainingTimeSeconds > 0) {
		pGame->setPaused(false);
	}
	else {
		pGame->printMessage("Cannot resume! Time is up.");
	}
}

SaveIcon::SaveIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {}

void SaveIcon::draw() const {
	drawButton(pGame->getWind(), RefPoint.x, RefPoint.y, width, height, BLUE, "Save");
}
void SaveIcon::onClick() {}

LoadIcon::LoadIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {}

void LoadIcon::draw() const {
	drawButton(pGame->getWind(), RefPoint.x, RefPoint.y, width, height, DARKBLUE, "Load");
}
void LoadIcon::onClick() {}

Toolbar::Toolbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	//First prepare List of images for each icon
	//To control the order of these images in the menu, reoder them in enum ICONS above	
	iconsImages[ICON_RESTART] = "images\\RESTART.jpg";
	iconsImages[ICON_PAUSE] = "images\\pause.png";
	iconsImages[ICON_RESUME] = "images\\resume.jpg";
	iconsImages[ICON_SAVE] = "images\\save.jpg";
	iconsImages[ICON_LOAD] = "images\\load.jpg";
	iconsImages[ICON_EXIT] = "images\\EXIT.jpg";
	point p;
	p.x = 0;
	p.y = 0;

	iconsList = new ToolbarIcon * [ICON_COUNT];

	//For each icon in the tool bar create an object 
	iconsList[ICON_RESTART] = new RestartIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_RESTART]);
	p.x += config.iconWidth;
	iconsList[ICON_PAUSE] = new PauseIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_PAUSE]);
	p.x += config.iconWidth;
	iconsList[ICON_RESUME] = new ResumeIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_RESUME]);
	p.x += config.iconWidth;
	iconsList[ICON_SAVE] = new SaveIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_SAVE]);
	p.x += config.iconWidth;
	iconsList[ICON_LOAD] = new LoadIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_LOAD]);
	p.x += config.iconWidth;
	iconsList[ICON_EXIT] = new ExitIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_EXIT]);
}

Toolbar::~Toolbar()
{
	for (int i = 0; i < ICON_COUNT; i++)
		delete iconsList[i];
	delete iconsList;
}

void Toolbar::draw() const
{
	for (int i = 0; i < ICON_COUNT; i++)
		iconsList[i]->draw();
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 3);
	pWind->DrawLine(0, config.toolBarHeight, pWind->GetWidth(), config.toolBarHeight);
}

bool Toolbar::handleClick(int x, int y)
{
	if (x > ICON_COUNT * config.iconWidth)	//click outside toolbar boundaries
		return false;


	//Check whick icon was clicked
	//==> This assumes that menu icons are lined up horizontally <==
	//Divide x co-ord of the point clicked by the icon width (int division)
	//if division result is 0 ==> first icon is clicked, if 1 ==> 2nd icon and so on

	int clickedIconIndex = (x / config.iconWidth);
	iconsList[clickedIconIndex]->onClick();	//execute onClick action of clicled icon

	if (clickedIconIndex == ICON_EXIT) return true;

	return false;

}

