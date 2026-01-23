#include "GameManager.h"
#include "Globals.h"
#include "Log.h"
#include "pdcpp/core/File.h"

GameManager::GameManager(PlaydateAPI* api)
: pd(api)
{
    LoadMaxScore();
    entityManager = std::make_unique<EntityManager>();
    ui = std::make_shared<UI>("/System/Fonts/Asheville-Sans-14-Bold.pft", entityManager.get());
    ui->SetOnNewGameSelected([this](){LoadNewGame();});
    ui->SetOnLoadGameSelected([this](){LoadSavedGame();});
    ui->SetOnGameOverSelected([this](){CleanGame();});
    ui->SetOnSaveGameSelected([this](){SaveGame();});
    ui->SetMaxScorePointer(&maxScore);
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
        // Check if player just leveled up and show popup
        if (player->JustLeveledUp() && !ui->IsLevelUpPopupShowing())
        {
            ui->ShowLevelUpPopup();
            player->ClearLevelUpFlag();
        }

        // Only tick game logic if level-up popup is not showing
        if (!ui->IsLevelUpPopupShowing() && player->IsAlive())
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
        player->DrawMagic(); // Draw magic projectiles on top of the map
        ui->SetOffset(drawOffset);
    }

    ui->Update();
    pd->system->drawFPS(0,0);
}

GameManager::~GameManager()
{
    // EntityManager is automatically cleaned up by unique_ptr
    SaveMaxScore();
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
    // First, check if save file exists and extract area ID
    unsigned int areaId = 0;
    if (!SaveGame::GetAreaIdFromSave(Globals::GAME_SAVE_PATH, areaId))
    {
        Log::Error("GameManager::LoadSavedGame - Failed to read save file or extract area ID");
        ui->SwitchScreen(GameScreen::MAIN_MENU); // Return to main menu if save file is invalid
        return;
    }

    // Get the area from entity manager based on saved area ID
    activeArea = std::static_pointer_cast<Area>(entityManager->GetEntity(areaId));
    if (!activeArea)
    {
        Log::Error("GameManager::LoadSavedGame - Area with ID %u not found in entity manager", areaId);
        ui->SwitchScreen(GameScreen::MAIN_MENU); // Return to main menu if area doesn't exist
        return;
    }

    // Setup the area
    activeArea->SetEntityManager(entityManager.get());
    activeArea->Load();

    // Create player
    player = std::make_shared<Player>();
    player->ResetStats(); // Initialize default stats (will be overwritten by save data)
    entityManager->SetPlayer(player);

    // Load save data into player and area
    if (!SaveGame::Load(player, activeArea, Globals::GAME_SAVE_PATH))
    {
        Log::Error("GameManager::LoadSavedGame - Failed to load save game data");
        // Clean up since load failed
        activeArea->Unload();
        activeArea.reset();
        player.reset();
        entityManager->SetPlayer(nullptr);
        ui->SwitchScreen(GameScreen::MAIN_MENU); // Return to main menu on failure
        return;
    }

    // Only set game as running if everything succeeded
    isGameRunning = true;
    Log::Info("GameManager::LoadSavedGame - Game loaded successfully from area %u", areaId);
}

void GameManager::SaveGame()
{
    if (!player || !activeArea) {
        Log::Error("Cannot save: player or area is null");
        return;
    }

    // Use new SaveGame framework
    if (!SaveGame::Save(player, activeArea, Globals::GAME_SAVE_PATH)) {
        Log::Error("Failed to save game");
    }
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

void GameManager::PauseGame() const
{
    ui->UpdateStatsMenuItem(player);
}

void GameManager::SaveMaxScore()
{
    auto fileHandle = std::make_unique<pdcpp::FileHandle>(Globals::MAX_SCORE_PATH, FileOptions::kFileWrite);

    if (!fileHandle) {
        Log::Error("Failed to open save file for writing");
        return;
    }

    size_t bufferSize = sizeof(int); // one single int
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);

    memcpy(buffer.get(), &maxScore, sizeof(int));

    int bytesWritten = fileHandle->write(buffer.get(), bufferSize);
    if (bytesWritten != static_cast<int>(bufferSize)) {
        Log::Error("Failed to write complete save data");
        return;
    }

    Log::Info("Max score saved successfully: %d", maxScore);
}

void GameManager::LoadMaxScore()
{
    if (!pdcpp::FileHelpers::fileExists(Globals::MAX_SCORE_PATH)) {
        maxScore = 0;
        return; // No file to load
    }

    auto fileHandle = std::make_unique<pdcpp::FileHandle>(Globals::MAX_SCORE_PATH, FileOptions::kFileReadData);

    // Check if file was opened successfully
    if (!fileHandle) {
        return;
    }

    size_t bufferSize = sizeof(int); // Max score is an int
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);

    int bytesRead = fileHandle->read(buffer.get(), bufferSize);
    if (bytesRead != static_cast<int>(bufferSize)) {
        Log::Error("Max score file corrupted or incomplete");
        return;
    }

    memcpy(&maxScore, buffer.get(), sizeof(int));
    Log::Info("Max score loaded successfully: %d", maxScore);
}

