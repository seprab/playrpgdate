//
// Created by Sergio Prada on 24/12/24.
//

#include "UI.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"
#include "pdcpp/graphics/Image.h"
#include "EntityManager.h"
#include "pdcpp/graphics/Graphics.h"
#include <pdcpp/graphics/LookAndFeel.h>
#include "pdcpp/graphics/Colors.h"
#include "UIConstants.h"

UI::UI(const char *fontPath, EntityManager* manager)
    : font(fontPath)  // Initialize pdcpp::Font with path
    , entityManager(manager)
{
    const char* err;

    const char* path = "images/ui/background.png";
    backgroundLoader = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(path, &err);
    if (backgroundLoader == nullptr)
        Log::Error("%s:%i Couldn't load background image: %s", __FILE__, __LINE__, path, err);

    path = "images/ui/gameui.png";
    gameOverlay = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(path, &err);
    if (gameOverlay == nullptr)
        Log::Error("%s:%i Couldn't load background image: %s", __FILE__, __LINE__, path, err);

    path = "images/ui/pause.png";
    pauseOverlay = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(path, &err);
    if (pauseOverlay == nullptr)
        Log::Error("%s:%i Couldn't load background image: %s", __FILE__, __LINE__, path, err);
    pdcpp::GlobalPlaydateAPI::get()->system->setMenuImage(pauseOverlay, 100);

    // Battery menu item will be created and updated by UpdateBatteryMenuItem()
    batteryMenuItem = nullptr;
    statsMenuItem = nullptr;

    magicIcons = {
            pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap("images/ui/icon_magic_beam", &err),
            pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap("images/ui/icon_magic_projectile", &err),
            pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap("images/ui/icon_magic_orbiting_projectile", &err)
    };
    if (magicIcons[0] == nullptr || magicIcons[1] == nullptr || magicIcons[2] == nullptr)
        Log::Error("Error loading magic icons: %s", err);

    playerFace = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap("images/ui/icon_player_face", &err);
    if (playerFace == nullptr)
        Log::Error("%s:%i Couldn't load background image: %s", __FILE__, __LINE__, path, err);

    magicCooldown = std::make_unique<CircularProgress>(0, 0, UIConstants::GameHUD::COOLDOWN_RADIUS);
    currentScreen = GameScreen::LOADING;
    loadingProgress = 0.0f;
    selectedMenuItem = 0;
}

void UI::Update()
{
    // Update battery and stats info every 1 second to avoid overhead
    float currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getElapsedTime();
    if (currentTime - lastBatteryUpdateTime >= 1.0f)
    {
        UpdateBatteryMenuItem();
        UpdateStatsMenuItem();
        lastBatteryUpdateTime = currentTime;
    }

    HandleInputs();
    Draw();
}

void UI::HandleInputs()
{
    if (pdcpp::GlobalPlaydateAPI::get()->system->getElapsedTime() < inputCooldown)
    {
        return; // Skip input processing if a cooldown period has not passed
    }
    PDButtons current;
    pdcpp::GlobalPlaydateAPI::get()->system->getButtonState(&current, nullptr, nullptr);

    switch (currentScreen)
    {
        default:
        case GameScreen::LOADING:
            if (loadingProgress >= 1.0f && (current))
            {
                SwitchScreen(GameScreen::MAIN_MENU);
                pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
            }
            break;
        case GameScreen::MAIN_MENU:
            if (current & kButtonUp)
            {
                selectedMenuItem = (selectedMenuItem - 1 + menuItemCount) % menuItemCount;
                pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
            }
            if (current & kButtonDown)
            {
                selectedMenuItem = (selectedMenuItem + 1) % menuItemCount;
                pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
            }
            if (current & kButtonA)
            {
                switch (selectedMenuItem)
                {
                    case 0: // New Game
                        pdcpp::GlobalPlaydateAPI::get()->graphics->clear(kColorBlack);
                        SwitchScreen(GameScreen::GAME);
                        if (newGameCallback) newGameCallback();
                        break;
                    case 1: // Load Game
                        pdcpp::GlobalPlaydateAPI::get()->graphics->clear(kColorBlack);
                        SwitchScreen(GameScreen::GAME);
                        if (loadGameCallback) loadGameCallback();
                        break;
                    default: break;
                }
                pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
            }
            break;
        case GameScreen::GAME:
            {
                std::shared_ptr<Player> player = entityManager->GetPlayer();
                if (!player->IsAlive())
                {
                    SwitchScreen(GameScreen::GAME_OVER);
                }
                break;
            }
        case GameScreen::GAME_OVER:
            {
                if (current & kButtonA)
                {
                    if (gameOverCallback) gameOverCallback();
                    SwitchScreen(GameScreen::MAIN_MENU);
                    pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
                }
                break;
            }
    }
}

void UI::Draw() const
{
    switch (currentScreen)
    {
        case GameScreen::LOADING:
            DrawLoadingScreen();
            break;
        case GameScreen::MAIN_MENU:
            DrawMainMenu();
            break;
        case GameScreen::GAME_OVER:
            DrawGameScreen();
            DrawGameOverScreen();
            break;
        case GameScreen::GAME:
            DrawGameScreen();
            break;
        default:
            break;
    }
}

void UI::DrawLoadingScreen() const
{
    using namespace UIConstants;
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();

    // Draw background image
    pd->graphics->drawBitmap(backgroundLoader, 0, 0, kBitmapUnflipped);

    // Draw black banner for text
    pdcpp::Rectangle<int> banner(
        0, Loading::BANNER_Y,
        SCREEN_WIDTH, Loading::BANNER_HEIGHT
    );
    pdcpp::Graphics::fillRectangle(banner, pdcpp::Colors::black);

    // Draw loading bar frame
    pdcpp::Rectangle<int> progressBarFrame(
        Loading::PROGRESS_BAR_X, Loading::PROGRESS_BAR_Y,
        Loading::PROGRESS_BAR_WIDTH, Loading::PROGRESS_BAR_HEIGHT
    );
    pdcpp::Graphics::drawRectangle(progressBarFrame, pdcpp::Colors::white);

    // Draw loading progress fill (with padding)
    int progressWidth = static_cast<int>(
        loadingProgress * (Loading::PROGRESS_BAR_WIDTH - 2 * Loading::PROGRESS_BAR_PADDING)
    );
    pdcpp::Rectangle<int> progressBar(
        Loading::PROGRESS_BAR_X + Loading::PROGRESS_BAR_PADDING,
        Loading::PROGRESS_BAR_Y + Loading::PROGRESS_BAR_PADDING,
        progressWidth,
        Loading::PROGRESS_BAR_HEIGHT - 2 * Loading::PROGRESS_BAR_PADDING
    );
    pdcpp::Graphics::fillRectangle(progressBar, pdcpp::Colors::white);

    // Animated loading text using Font::drawWrappedText
    static int frameCount = 0;
    std::string loadingText;
    frameCount++;

    // Create text bounds for centered text
    pdcpp::Rectangle<float> textBounds(
        0, Loading::TEXT_Y,
        SCREEN_WIDTH, static_cast<float>(font.getFontHeight())
    );

    if (loadingProgress < 1.0f)
    {
        // Simple dots animation
        switch ((frameCount / Loading::ANIMATION_FRAME_DELAY) % 3)
        {
            case 0: loadingText = "."; break;
            case 1: loadingText = ".."; break;
            case 2: loadingText = "..."; break;
            default: loadingText = ""; break;
        }

        // Set draw mode to white text on black background
        SetTextDrawMode(Theme::TEXT_COLOR);
        font.drawWrappedText(
            loadingText,
            textBounds,
            pdcpp::Font::Center,
            pdcpp::Font::Top
        );
        pd->graphics->setDrawMode(kDrawModeCopy);
    }
    else
    {
        // Blinking "press button" message
        switch ((frameCount / Loading::ANIMATION_FRAME_DELAY) % 4)
        {
            case 0: loadingText = "Press any button to continue."; break;
            case 1: loadingText = "Press any button to continue.."; break;
            case 2: loadingText = "Press any button to continue..."; break;
            case 3:
            default: loadingText = ""; break;
        }

        // Set draw mode to white text on black background
        SetTextDrawMode(Theme::TEXT_COLOR);
        font.drawWrappedText(
            loadingText,
            textBounds,
            pdcpp::Font::Center,
            pdcpp::Font::Top
        );
        pd->graphics->setDrawMode(kDrawModeCopy);
    }
}

void UI::DrawMainMenu() const
{
    using namespace UIConstants;
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();

    // Draw background image
    pd->graphics->drawBitmap(backgroundLoader, 0, 0, kBitmapUnflipped);

    // Draw menu panel with rounded corners for a nicer look
    pdcpp::Rectangle<int> panel(
        MainMenu::PANEL_X, MainMenu::PANEL_Y,
        MainMenu::PANEL_WIDTH, MainMenu::PANEL_HEIGHT
    );
    pdcpp::Graphics::fillRoundedRectangle(panel, 8, pdcpp::Colors::black);

    // Draw title using Font::drawWrappedText for automatic centering
    pdcpp::Rectangle<float> titleBounds(
        0, MainMenu::TITLE_Y,
        SCREEN_WIDTH, static_cast<float>(font.getFontHeight())
    );
    SetTextDrawMode(Theme::TEXT_COLOR);
    font.drawWrappedText(
        "CardoBlast",
        titleBounds,
        pdcpp::Font::Center,
        pdcpp::Font::Top
    );
    pd->graphics->setDrawMode(kDrawModeCopy);

    // Draw menu items
    for (int i = 0; i < menuItemCount; i++)
    {
        int itemY = MainMenu::MENU_START_Y + i * MainMenu::MENU_ITEM_SPACING;

        // Create bounds for this menu item
        pdcpp::Rectangle<int> itemRect(
            MainMenu::MENU_ITEM_X, itemY,
            MainMenu::MENU_ITEM_WIDTH, MainMenu::MENU_ITEM_HEIGHT
        );

        // Add horizontal padding for text (10px left, 10px right)
        pdcpp::Rectangle<float> textBounds = itemRect.toFloat();
        textBounds.x += 10;
        textBounds.width -= 20;

        if (i == selectedMenuItem)
        {
            // Draw selection highlight with rounded corners
            pdcpp::Graphics::fillRoundedRectangle(itemRect, 4, pdcpp::Colors::white);

            // Draw text in inverse color (black on white) with padding
            pd->graphics->setDrawMode(kDrawModeCopy); // Black text on white background
            font.drawWrappedText(
                menuItems[i],
                textBounds,
                pdcpp::Font::Left,
                pdcpp::Font::Top
            );
        }
        else
        {
            // Draw unselected text with padding
            SetTextDrawMode(Theme::TEXT_COLOR);
            font.drawWrappedText(
                menuItems[i],
                textBounds,
                pdcpp::Font::Left,
                pdcpp::Font::Top
            );
            pd->graphics->setDrawMode(kDrawModeCopy);
        }
    }
}

void UI::DrawGameScreen() const
{
    using namespace UIConstants;
    std::shared_ptr<Player> player = entityManager->GetPlayer();
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();

    // Draw game overlay HUD
    pd->graphics->drawBitmap(
        gameOverlay,
        offset.x + GameHUD::OVERLAY_OFFSET_X,
        offset.y + GameHUD::OVERLAY_OFFSET_Y,
        kBitmapUnflipped
    );

    // Draw player coordinates (avoid first frame to prevent drawing off-screen)
    if (offset.x != 0 && offset.y != 0)
    {
        pdcpp::Point<int> playerTiledPos = player->GetTiledPosition();
        char posX[3], posY[3];
        snprintf(posX, sizeof(posX), "%d", playerTiledPos.x);
        snprintf(posY, sizeof(posY), "%d", playerTiledPos.y);

        SetTextDrawMode(Theme::TEXT_COLOR);
        pd->graphics->drawText(
            posX, strlen(posX), kASCIIEncoding,
            offset.x + GameHUD::COORDS_X_OFFSET,
            offset.y + GameHUD::COORDS_Y_OFFSET_Y
        );
        pd->graphics->drawText(
            posY, strlen(posY), kASCIIEncoding,
            offset.x + GameHUD::COORDS_Y_OFFSET_X,
            offset.y + GameHUD::COORDS_Y_OFFSET_Y
        );
        pd->graphics->setDrawMode(kDrawModeCopy);
    }

    // Draw magic cooldown indicator
    magicCooldown->UpdatePosition(
        offset.x,
        offset.y + GameHUD::MAGIC_COOLDOWN_Y_OFFSET
    );
    magicCooldown->Draw(player->GetCooldownPercentage());

    // Draw magic icons (previous, current, next)
    const unsigned int activeMagic = player->GetSelectedMagic();
    const int iconCount = static_cast<int>(magicIcons.size());
    const int prevIndex = (activeMagic == 0) ? iconCount - 1 : activeMagic - 1;
    const int currIndex = static_cast<int>(activeMagic);
    const int nextIndex = (activeMagic + 1) % iconCount;

    pd->graphics->drawBitmap(
        magicIcons[prevIndex],
        offset.x + GameHUD::MAGIC_ICON_LEFT_X_OFFSET,
        offset.y + GameHUD::MAGIC_ICON_LEFT_Y_OFFSET,
        kBitmapUnflipped
    );
    pd->graphics->drawBitmap(
        magicIcons[currIndex],
        offset.x + GameHUD::MAGIC_ICON_CENTER_X_OFFSET,
        offset.y + GameHUD::MAGIC_ICON_CENTER_Y_OFFSET,
        kBitmapUnflipped
    );
    pd->graphics->drawBitmap(
        magicIcons[nextIndex],
        offset.x + GameHUD::MAGIC_ICON_RIGHT_X_OFFSET,
        offset.y + GameHUD::MAGIC_ICON_RIGHT_Y_OFFSET,
        kBitmapUnflipped
    );

    // Draw player face icon
    pd->graphics->drawBitmap(
        playerFace,
        offset.x + GameHUD::PLAYER_FACE_X_OFFSET,
        offset.y + GameHUD::PLAYER_FACE_Y_OFFSET,
        kBitmapUnflipped
    );
}

void UI::DrawGameOverScreen() const
{
    using namespace UIConstants;

    // Draw semi-transparent panel with rounded corners
    pdcpp::Rectangle<int> panelRectA(
        offset.x + GameOver::PANELA_OFFSET_X,
        offset.y + GameOver::PANELA_OFFSET_Y,
        GameOver::PANELA_WIDTH,
        GameOver::PANELA_HEIGHT
    );
    pdcpp::Graphics::fillRoundedRectangle(panelRectA, 8, pdcpp::Colors::transparent50GrayB);

    pdcpp::Rectangle<int> panelRectB(
        offset.x + GameOver::PANELB_OFFSET_X,
        offset.y + GameOver::PANELB_OFFSET_Y,
        GameOver::PANELB_WIDTH,
        GameOver::PANELB_HEIGHT
    );
    pdcpp::Graphics::fillRoundedRectangle(panelRectB, 8, pdcpp::Colors::black);

    // Get player stats
    std::shared_ptr<Player> player = entityManager->GetPlayer();
    unsigned int kills = player->GetMonstersKilled();
    unsigned int survivalTime = player->GetSurvivalTimeSeconds();
    unsigned int minutes = survivalTime / 60;
    unsigned int seconds = survivalTime % 60;

    // Format the stats text
    char statsBuffer[128];
    snprintf(statsBuffer, sizeof(statsBuffer),
        "Game Over\n\nMonsters Killed: %u\nTime Survived: %u:%02u\n\nPress A to return to the main menu.",
        kills, minutes, seconds);

    // Set draw mode to white text
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();
    SetTextDrawMode(Theme::TEXT_COLOR);

    // The text will be centered both horizontally and vertically in the panel
    font.drawWrappedText(
        statsBuffer,
        panelRectB.toFloat(),
        pdcpp::Font::Center,   // Horizontally centered
        pdcpp::Font::Middle    // Vertically centered
    );

    pd->graphics->setDrawMode(kDrawModeCopy);
}

void UI::SwitchScreen(GameScreen newScreen)
{
    if (newScreen == GameScreen::GAME)
    {
        saveMenuItem = pdcpp::GlobalPlaydateAPI::get()->system->addMenuItem("Save", SaveGameCallback, this);
        backToStartMenuItem = pdcpp::GlobalPlaydateAPI::get()->system->addMenuItem("End run", HomeMenuCallback, this);

        // Stats menu item will be created by UpdateStatsMenuItem() when player exists
        statsMenuItem = nullptr;
    }
    else
    {
        if (saveMenuItem)
        {
            pdcpp::GlobalPlaydateAPI::get()->system->removeMenuItem(saveMenuItem);
            saveMenuItem = nullptr;
        }
        if (backToStartMenuItem)
        {
            pdcpp::GlobalPlaydateAPI::get()->system->removeMenuItem(backToStartMenuItem);
            backToStartMenuItem = nullptr;
        }
        if (statsMenuItem)
        {
            pdcpp::GlobalPlaydateAPI::get()->system->removeMenuItem(statsMenuItem);
            statsMenuItem = nullptr;
        }
    }
    currentScreen = newScreen;
}

void UI::UpdateLoadingProgress(float progress)
{
    loadingProgress = progress;
    if (loadingProgress > 1.0f) loadingProgress = 1.0f;
    if (loadingProgress < 0.0f) loadingProgress = 0.0f;
}

void UI::UpdateBatteryMenuItem()
{
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();

    // Get current battery status
    float percentage = pd->system->getBatteryPercentage();
    float voltage = pd->system->getBatteryVoltage();

    // Format the battery info strings
    char percentageStr[32];
    char voltageStr[32];
    snprintf(percentageStr, sizeof(percentageStr), "%.0f%%", percentage);
    snprintf(voltageStr, sizeof(voltageStr), "%.2fV", voltage);

    // Remove old menu item if it exists
    if (batteryMenuItem)
    {
        pd->system->removeMenuItem(batteryMenuItem);
        batteryMenuItem = nullptr;
    }

    // Create new menu item with updated values
    // Note: We need to keep these strings alive, so we use static storage
    static char option1[32];
    static char option2[32];
    snprintf(option1, sizeof(option1), "%s", percentageStr);
    snprintf(option2, sizeof(option2), "%s", voltageStr);

    const char* battery_options[] = {option1, option2};
    batteryMenuItem = pd->system->addOptionsMenuItem(
        "Battery",
        battery_options, 2,
        nullptr,
        nullptr);
}

void UI::UpdateStatsMenuItem()
{
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();

    // Only update stats if we're in game modes
    if (currentScreen != GameScreen::GAME)
    {
        Log::Info("UpdateStatsMenuItem: Not in GAME mode, current screen = %d", static_cast<int>(currentScreen));
        return;
    }

    std::shared_ptr<Player> player = entityManager->GetPlayer();

    // If player doesn't exist yet, remove stats menu if it exists
    if (!player)
    {
        Log::Info("UpdateStatsMenuItem: Player doesn't exist yet");
        if (statsMenuItem)
        {
            pd->system->removeMenuItem(statsMenuItem);
            statsMenuItem = nullptr;
        }
        return;
    }

    Log::Info("UpdateStatsMenuItem: Creating stats menu item");

    // Remove old menu item if it exists
    if (statsMenuItem)
    {
        pd->system->removeMenuItem(statsMenuItem);
        statsMenuItem = nullptr;
    }

    // Format the stats strings with static storage
    static char hp[32];
    static char kills[32];
    static char time[32];
    snprintf(hp, sizeof(hp), "HP: %d", player->GetHP());
    snprintf(kills, sizeof(kills), "Kills: %u", player->GetMonstersKilled());
    snprintf(time, sizeof(time), "Time: %.0fs", pd->system->getElapsedTime());

    const char* stats_options[] = {hp, kills, time};
    statsMenuItem = pd->system->addOptionsMenuItem(
        "Stats",
        stats_options, 3,
        nullptr,
        nullptr);

    Log::Info("UpdateStatsMenuItem: Stats menu created, pointer = %p", statsMenuItem);
}

// Static callback functions for menu items
void UI::SaveGameCallback(void* userdata)
{
    UI* ui = static_cast<UI*>(userdata);
    if (ui && ui->saveGameCallback)
    {
        ui->saveGameCallback();
    }
}

void UI::HomeMenuCallback(void* userdata)
{
    UI* ui = static_cast<UI*>(userdata);
    if (ui)
    {
        ui->SwitchScreen(GameScreen::GAME_OVER);
    }
}

// Helper methods implementation
void UI::SetTextDrawMode(LCDColor color) const
{
    if (color == kColorWhite)
        pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode(kDrawModeFillWhite);
    else
        pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode(kDrawModeCopy);
}
