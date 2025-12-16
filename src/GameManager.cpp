#include "GameManager.h"
#include "Globals.h"
#include "Log.h"
#include "pdcpp/core/File.h"

GameManager::GameManager(PlaydateAPI* api)
: pd(api)
{
    entityManager = std::make_unique<EntityManager>();
    ui = std::make_shared<UI>("/System/Fonts/Asheville-Sans-14-Bold.pft", entityManager.get());
    ui->SetOnNewGameSelected([this](){LoadNewGame();});
    ui->SetOnLoadGameSelected([this](){LoadSavedGame();});
    ui->SetOnGameOverSelected([this](){CleanGame();});
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
                entityManager->LoadJSON<Item>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 1:
                entityManager->LoadJSON<Door>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 2:
                entityManager->LoadJSON<Weapon>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 3:
                entityManager->LoadJSON<Armor>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 4:
                entityManager->LoadJSON<Monster>(jsonPaths[frameCount], tokenCount[frameCount]);
                break;
            case 5:
                entityManager->LoadJSON<Area>(jsonPaths[frameCount], tokenCount[frameCount]);
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
        if (player->IsAlive())
        {
            player->Tick(activeArea);
            activeArea->Tick(player.get());
        }

        // Calculate camera position
        float cameraSpeed = 0.2f; // Adjust for smoothness
        currentCameraOffset.x = static_cast<int>(static_cast<float>(currentCameraOffset.x) +
            static_cast<float>(player->GetPosition().x - currentCameraOffset.x) * cameraSpeed);
        currentCameraOffset.y = static_cast<int>(static_cast<float>(currentCameraOffset.y) +
            static_cast<float>(player->GetPosition().y - currentCameraOffset.y) * cameraSpeed);

        // Move the camera to the calculated position
        int screenCenterWidth = pd->display->getWidth() / 2;
        int screenCenterHeight = pd->display->getHeight() / 2;

        pdcpp::Point<int> drawOffset = {0,0};
        if (currentCameraOffset.x > screenCenterWidth)
        {
            if (currentCameraOffset.x > (activeArea->GetWidth() * Globals::MAP_TILE_SIZE) - screenCenterWidth)
            {
                drawOffset.x = -((activeArea->GetWidth() * Globals::MAP_TILE_SIZE) - pd->display->getWidth());
            }
            else
            {
                drawOffset.x = -currentCameraOffset.x + screenCenterWidth;
            }
        }
        if (currentCameraOffset.y > screenCenterHeight)
        {
            if (currentCameraOffset.y > (activeArea->GetHeight() * Globals::MAP_TILE_SIZE) - screenCenterHeight)
            {
                drawOffset.y = -((activeArea->GetHeight() * Globals::MAP_TILE_SIZE) - pd->display->getHeight());
            }
            else
            {
                drawOffset.y = -currentCameraOffset.y + screenCenterHeight;
            }
        }
        pd->graphics->setDrawOffset(drawOffset.x, drawOffset.y);
        drawOffset.x = -drawOffset.x+screenCenterWidth;
        drawOffset.y = -drawOffset.y+screenCenterHeight;


        activeArea->Render(drawOffset.x, drawOffset.y, Globals::PLAYER_FOV_X, Globals::PLAYER_FOV_Y);
        player->Draw();
        ui->SetOffset(drawOffset);
    }

    ui->Update();
    pd->system->drawFPS(0,0);
}

GameManager::~GameManager()
{
    if (player && player->IsAlive())
    {
        SaveGame();
    }

    // EntityManager is automatically cleaned up by unique_ptr
    Log::Info("GameManager destroyed");
}

void GameManager::LoadNewGame()
{
    activeArea = std::static_pointer_cast<Area>(entityManager->GetEntity(9002));
    activeArea->SetEntityManager(entityManager.get());
    activeArea->Load();

    player = std::make_shared<Player>();
    player->SetTiledPosition(pdcpp::Point<int>(23, 32));
    player->ResetStats(); // Initialize game stats
    entityManager->SetPlayer(player);
    isGameRunning = true;
}

void GameManager::LoadSavedGame()
{
    activeArea = std::static_pointer_cast<Area>(entityManager->GetEntity(9002));
    activeArea->SetEntityManager(entityManager.get());
    activeArea->Load();

    player = std::make_shared<Player>();
    player->SetTiledPosition(pdcpp::Point<int>(23, 32));
    player->ResetStats(); // Initialize game stats
    entityManager->SetPlayer(player);
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

void GameManager::CleanGame()
{
    currentCameraOffset = {0,0};
    pd->graphics->setDrawOffset(currentCameraOffset.x, currentCameraOffset.y);
    ui->SetOffset(currentCameraOffset);
    isGameRunning = false;

    // Clean up the active area before releasing it
    if (activeArea != nullptr)
    {
        activeArea->Unload();
        activeArea = nullptr;
    }

    player = nullptr;
}

