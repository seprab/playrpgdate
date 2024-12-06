#include "GameManager.h"
#include "Log.h"

GameManager::GameManager(PlaydateAPI* api)
: fontpath("/System/Fonts/Asheville-Sans-14-Bold.pft")
, x((400 - TEXT_WIDTH) / 2)
, y((240 - TEXT_HEIGHT) / 2)
, dx(1)
, dy(2) , pd(api)
{
    Log::Initialize(pd);
    const char* err;
    font = pd->graphics->loadFont(fontpath.c_str(), &err);

    if (font == nullptr)
        Log::Error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath.c_str(), err);

    new EntityManager(api);

}
void GameManager::Update()
{
    pd->graphics->clear(kColorWhite);
    pd->graphics->setFont(font);
    pd->graphics->drawText("SepraB!", strlen("Hello World!"), kASCIIEncoding, x, y);

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

GameManager::~GameManager()
{
    delete EntityManager::GetInstance();
    Log::Info("GameManager destroyed");
}
