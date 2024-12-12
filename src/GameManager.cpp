#include "GameManager.h"
#include "Log.h"

GameManager::GameManager(PlaydateAPI* api)
: fontpath("/System/Fonts/Asheville-Sans-14-Bold.pft"), pd(api)
{
    Log::Initialize(pd);
    const char* err;
    font = pd->graphics->loadFont(fontpath.c_str(), &err);

    if (font == nullptr)
        Log::Error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath.c_str(), err);

    new EntityManager(api);
    player = new Player();
    map = new Map();
    map->LoadLayers("data/samplemap.json", 7510);
    map->LoadImageTable("images/maps/gray/tilesetworld.pdt");
    map->SetMapScale(1);
}
void GameManager::Update()
{
    pd->graphics->clear(kColorWhite);
    pd->graphics->setFont(font);
    pd->graphics->drawText("o", strlen("o"), kASCIIEncoding, 0, 0);


    map->Render(player->GetPosition().first, player->GetPosition().second, 180);
    player->Tick();

    // Calculate camera position
    float cameraSpeed = 0.2f; // Adjust for smoothness
    currentCameraOffset.first = currentCameraOffset.first + (player->GetPosition().first - currentCameraOffset.first) * cameraSpeed;
    currentCameraOffset.second = currentCameraOffset.second + (player->GetPosition().second - currentCameraOffset.second)  * cameraSpeed;


    // Move the camera to calculated position
    pd->graphics->setDrawOffset(-currentCameraOffset.first + 200, -currentCameraOffset.second + 120);

    // From here, I should start drawing the game UI. Otherwise, the camera will move the UI as well.

    // Show player coordinates in the top-right of the screen, x and y in separate rects to make it easier to read
    const char* resultX = std::to_string(currentCameraOffset.first).c_str();
    const char* resultY = std::to_string(currentCameraOffset.second).c_str();
    //pd->graphics->drawRect(currentCameraOffset.first + 100, currentCameraOffset.second - 120, 50, 15, kColorBlack);
    //pd->graphics->drawRect(currentCameraOffset.first + 150, currentCameraOffset.second - 120, 50, 15, kColorBlack);
    pd->graphics->drawTextInRect(resultX, strlen(resultX), kASCIIEncoding, currentCameraOffset.first + 100, currentCameraOffset.second - 120, 50, 15,PDTextWrappingMode::kWrapWord, PDTextAlignment::kAlignTextRight);
    pd->graphics->drawTextInRect(resultY, strlen(resultY), kASCIIEncoding, currentCameraOffset.first + 150, currentCameraOffset.second - 120, 50, 15,PDTextWrappingMode::kWrapWord, PDTextAlignment::kAlignTextRight);



    pd->system->drawFPS(0,0);
}

GameManager::~GameManager()
{
    delete EntityManager::GetInstance();
    Log::Info("GameManager destroyed");
}
