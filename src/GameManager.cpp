#include "GameManager.h"
#include "Log.h"
#include "pdcpp/core/File.h"

GameManager::GameManager(PlaydateAPI* api)
: pd(api)
{
    ui = std::make_shared<UI>("/System/Fonts/Asheville-Sans-14-Bold.pft");
    ui->SetOnNewGameSelected([this](){LoadNewGame();});
    ui->SetOnLoadGameSelected([this](){LoadSavedGame();});
    new EntityManager();
}
void GameManager::Update()
{
    pd->graphics->clear(kColorBlack);


    if (!isGameRunning)
    {
        // I'm loading jsons here to create a loading bar. When a json load is complete, update the loading bar.
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
        activeArea->Render(player->GetPosition().x, player->GetPosition().y, 240, 160);
        player->Tick(activeArea);

        // Calculate camera position
        float cameraSpeed = 0.2f; // Adjust for smoothness
        currentCameraOffset.first = currentCameraOffset.first + (player->GetPosition().x - currentCameraOffset.first) * cameraSpeed;
        currentCameraOffset.second = currentCameraOffset.second + (player->GetPosition().y - currentCameraOffset.second)  * cameraSpeed;

        // Move the camera to calculated position
        pd->graphics->setDrawOffset(-currentCameraOffset.first + 200, -currentCameraOffset.second + 120);
        ui->SetOffset(currentCameraOffset);
    }

    ui->Update();
    pd->system->drawFPS(0,0);
}

GameManager::~GameManager()
{
    SaveGame();
    delete EntityManager::GetInstance();
    Log::Info("GameManager destroyed");
}

void GameManager::LoadNewGame()
{
    activeArea = std::static_pointer_cast<Area>(EntityManager::GetInstance()->GetEntity(9002));
    activeArea->Load();

    player = std::make_shared<Player>();
    player->SetPosition(pdcpp::Point<int>(89*activeArea->GetTileWidth(), 145*activeArea->GetTileHeight()));
    EntityManager::GetInstance()->SetPlayer(player);
    isGameRunning = true;
}

void GameManager::LoadSavedGame()
{
    activeArea = std::static_pointer_cast<Area>(EntityManager::GetInstance()->GetEntity(9002));
    activeArea->Load();

    player = std::make_shared<Player>();
    player->SetPosition(pdcpp::Point<int>(89*activeArea->GetTileWidth(), 145*activeArea->GetTileHeight()));
    EntityManager::GetInstance()->SetPlayer(player);
    isGameRunning = true;

    const char* savePath = "saves/save.data";
    auto fileHandle = std::make_unique<pdcpp::FileHandle>(savePath, FileOptions::kFileRead);
    size_t bufferSize = sizeof(player->GetPosition().x) + sizeof(player->GetPosition().y);
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);
    fileHandle->read(buffer.get(), bufferSize);
    pdcpp::Point<int> position = pdcpp::Point<int>(0,0);
    memcpy(&position.x, buffer.get(), sizeof(position.x));
    memcpy(&position.y, buffer.get() + sizeof(position.x), sizeof(position.y));
    player->SetPosition(position);
}

void GameManager::SaveGame()
{
    const char* savePath = "saves/save.data";
    auto fileHandle = std::make_unique<pdcpp::FileHandle>(savePath, FileOptions::kFileWrite);
    size_t bufferSize = sizeof(player->GetPosition().x) + sizeof(player->GetPosition().y);
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);
    pdcpp::Point<int> position = player->GetPosition();
    memcpy(buffer.get(), &position.x, sizeof(position.x));
    memcpy(buffer.get() + sizeof(position.x), &position.y, sizeof(position.y));
    fileHandle->write(buffer.get(), bufferSize);
    Log::Info("Game saved");
}
