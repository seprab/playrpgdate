#include "GameManager.h"
#include "Log.h"

GameManager::GameManager(PlaydateAPI* api)
: pd(api)
{
    Log::Initialize(pd);

    ui = std::make_shared<UI>("/System/Fonts/Asheville-Sans-14-Bold.pft");
    ui->SetOnNewGameSelected([this](){LoadNewGame();});

    new EntityManager(api);
    EntityManager::GetInstance()->PreloadEntities(ui);
}
void GameManager::Update()
{
    pd->graphics->clear(kColorBlack);


    if(isGameRunning)
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
    ui->Draw();

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
