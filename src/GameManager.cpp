#include "GameManager.h"
#include "Log.h"

GameManager::GameManager(PlaydateAPI* api)
: pd(api)
{
    ui = std::make_shared<UI>("/System/Fonts/Asheville-Sans-14-Bold.pft");
    ui->SetOnNewGameSelected([this](){LoadNewGame();});

    new EntityManager();
}
void GameManager::Update()
{
    pd->graphics->clear(kColorBlack);


    if (!isGameRunning)
    {
        int tokenCount[] {128, 64, 64, 128, 2300, 128};
        const char* jsonPaths[] {"data/items.json", "data/doors.json", "data/weapons.json", "data/armors.json", "data/creatures.json", "data/areas.json"};
        float totalTokens = 128.f + 64.f + 64.f + 128.f + 2300.f + 128.f;

        static float progress = 0.0f;
        static int frameCount = 0;

        switch (frameCount)
        {
            case 0:
                EntityManager::GetInstance()->LoadJSON<Item>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 1:
                EntityManager::GetInstance()->LoadJSON<Door>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 2:
                EntityManager::GetInstance()->LoadJSON<Weapon>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 3:
                EntityManager::GetInstance()->LoadJSON<Armor>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 4:
                EntityManager::GetInstance()->LoadJSON<Creature>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 5:
                EntityManager::GetInstance()->LoadJSON<Area>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            default:
                progress = 1.f;
                break;
        }
        if (frameCount < sizeof(tokenCount)/sizeof(tokenCount[0]))
        {
            progress += (float)tokenCount[frameCount] / totalTokens;
            ui->UpdateLoadingProgress(progress);
            Log::Info("FrameCount: %d, Progress: %f", frameCount, progress);
            frameCount++;
        }
    }
    else
    {
        activeArea->Render(player->GetPosition().first, player->GetPosition().second, 240, 160);
        player->Tick(activeArea);

        // Calculate camera position
        float cameraSpeed = 0.2f; // Adjust for smoothness
        currentCameraOffset.first = currentCameraOffset.first + (player->GetPosition().first - currentCameraOffset.first) * cameraSpeed;
        currentCameraOffset.second = currentCameraOffset.second + (player->GetPosition().second - currentCameraOffset.second)  * cameraSpeed;

        // Move the camera to calculated position
        pd->graphics->setDrawOffset(-currentCameraOffset.first + 200, -currentCameraOffset.second + 120);
        ui->SetOffset(currentCameraOffset);
    }

    ui->Update();
    pd->system->drawFPS(0,0);
}

GameManager::~GameManager()
{
    delete EntityManager::GetInstance();
    Log::Info("GameManager destroyed");
}

void GameManager::LoadNewGame()
{
    activeArea = std::static_pointer_cast<Area>(EntityManager::GetInstance()->GetEntity(9002));
    activeArea->Load();

    player = new Player();
    player->SetPosition(std::pair<int,int>(89*activeArea->GetTileWidth(), 145*activeArea->GetTileHeight()));
    isGameRunning = true;
}

void GameManager::LoadSavedGame()
{

}
