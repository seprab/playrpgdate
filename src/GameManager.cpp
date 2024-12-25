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


    // ************ This should be the code for a new game ************
    activeArea = std::static_pointer_cast<Area>(EntityManager::GetInstance()->GetEntity(9002));
    activeArea->Load();

    player = new Player();
    player->SetPosition(std::pair<int,int>(89*activeArea->GetTileWidth(), 145*activeArea->GetTileHeight()));
    // ************ ************ ************ ************  ************
}
void GameManager::Update()
{
    pd->graphics->clear(kColorBlack);
    pd->graphics->setFont(font);

    activeArea->Render(player->GetPosition().first, player->GetPosition().second, 240, 160);
    player->Tick(activeArea);

    // Calculate camera position
    float cameraSpeed = 0.2f; // Adjust for smoothness
    currentCameraOffset.first = currentCameraOffset.first + (player->GetPosition().first - currentCameraOffset.first) * cameraSpeed;
    currentCameraOffset.second = currentCameraOffset.second + (player->GetPosition().second - currentCameraOffset.second)  * cameraSpeed;


    // Move the camera to calculated position
    pd->graphics->setDrawOffset(-currentCameraOffset.first + 200, -currentCameraOffset.second + 120);

    // From here, I should start drawing the game UI. Otherwise, the camera will move the UI as well.

    // Show player coordinates in the top-right of the screen, x and y in separate rects to make it easier to read
    char resultX[4], resultY[4];
    snprintf(resultX, sizeof(resultX), "%d", currentCameraOffset.first);
    snprintf(resultY, sizeof(resultY), "%d", currentCameraOffset.second);
    pd->graphics->drawRect(currentCameraOffset.first + 100, currentCameraOffset.second - 120, 50, 15, kColorWhite);
    pd->graphics->drawRect(currentCameraOffset.first + 150, currentCameraOffset.second - 120, 50, 15, kColorWhite);

    pd->graphics->fillRect(currentCameraOffset.first + 102, currentCameraOffset.second - 118, 46, 11, kColorWhite);
    pd->graphics->fillRect(currentCameraOffset.first + 152, currentCameraOffset.second - 118, 46, 11, kColorWhite);

    pd->graphics->drawTextInRect(resultX, strlen(resultX), kASCIIEncoding, currentCameraOffset.first + 100, currentCameraOffset.second - 120, 50, 15,PDTextWrappingMode::kWrapWord, PDTextAlignment::kAlignTextRight);
    pd->graphics->drawTextInRect(resultY, strlen(resultY), kASCIIEncoding, currentCameraOffset.first + 150, currentCameraOffset.second - 120, 50, 15,PDTextWrappingMode::kWrapWord, PDTextAlignment::kAlignTextRight);


    pd->system->drawFPS(0,0);
}

GameManager::~GameManager()
{
    delete EntityManager::GetInstance();
    Log::Info("GameManager destroyed");
}
