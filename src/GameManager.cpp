//
// Created by Sergio Prada on 16/06/24.
//

#include "GameManager.h"

GameManager::GameManager(PlaydateAPI* api)
: pd(api)
, fontpath("/System/Fonts/Asheville-Sans-14-Bold.pft")
, x((400 - TEXT_WIDTH) / 2)
, y((240 - TEXT_HEIGHT) / 2)
, dx(1) , dy(2)
{
    const char* err;
    font = pd->graphics->loadFont(fontpath.c_str(), &err);

    if (font == nullptr)
        pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath.c_str(), err);
}
void GameManager::Update()
{
    pd->graphics->clear(kColorWhite);
    pd->graphics->setFont(font);
    pd->graphics->drawText("Hellooo!", strlen("Hello World!"), kASCIIEncoding, x, y);

    x += dx;
    y += dy;

    if ( x < 0 || x > LCD_COLUMNS - TEXT_WIDTH )
    {
        dx = -dx;
    }

    if ( y < 0 || y > LCD_ROWS - TEXT_HEIGHT )
    {
        dy = -dy;
    }

    pd->system->drawFPS(0,0);
}