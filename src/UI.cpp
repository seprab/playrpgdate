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

    statsMenuItem = nullptr;

    magicIcons.clear();
    magicIconPaths.clear();

    playerFace = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap("images/ui/icon_player_face", &err);
    if (playerFace == nullptr)
        Log::Error("%s:%i Couldn't load background image: %s", __FILE__, __LINE__, path, err);

    magicCooldown = std::make_unique<CircularProgress>(0, 0, UIConstants::GameHUD::COOLDOWN_RADIUS);
    currentScreen = GameScreen::LOADING;
    loadingProgress = 0.0f;
    selectedMenuItem = 0;

    // Initialize level-up popup state
    showLevelUpPopup = false;
    levelUpSelectedOption = 0;
    pendingLevelUps = 0;
}

void UI::Update()
{
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
                pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
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
            }
            break;
        case GameScreen::GAME:
            {
                // Handle level-up popup input if it's showing
                if (showLevelUpPopup)
                {
                    std::shared_ptr<Player> player = entityManager->GetPlayer();
                    if (!player)
                    {
                        break;
                    }

                    if (current & kButtonUp)
                    {
                        levelUpSelectedOption = (levelUpSelectedOption - 1 + 4) % 4;
                        pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
                    }
                    else if (current & kButtonDown)
                    {
                        levelUpSelectedOption = (levelUpSelectedOption + 1) % 4;
                        pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
                    }
                    else if (current & kButtonA)
                    {
                        // Spend the skill point based on selection
                        switch (levelUpSelectedOption)
                        {
                            case 0:
                                player->SpendSkillPoint(Player::StatType::Strength);
                                break;
                            case 1:
                                player->SpendSkillPoint(Player::StatType::Agility);
                                break;
                            case 2:
                                player->SpendSkillPoint(Player::StatType::Constitution);
                                break;
                            case 3:
                                player->SpendSkillPoint(Player::StatType::MaxHp);
                                break;
                            default:
                                break;
                        }

                        // Check if there are more skill points to spend
                        if (player->GetSkillPoints() > 0)
                        {
                            // Reset selection for next point
                            levelUpSelectedOption = 0;
                        }
                        else
                        {
                            // Close popup when all skill points are spent
                            showLevelUpPopup = false;
                            pendingLevelUps = 0;
                        }

                        pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
                    }
                    break; // Don't process game input while popup is showing
                }

                // Normal game input handling
                std::shared_ptr<Player> player = entityManager->GetPlayer();
                if (!player->IsAlive())
                {
                    if (player->GetMonstersKilled() > *maxScorePtr)
                    {
                        *maxScorePtr = player->GetMonstersKilled();
                    }
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
            // Draw level-up popup on top of the game screen if it's showing
            if (showLevelUpPopup)
            {
                DrawLevelUpPopup();
            }
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

    // Draw compact background panel for loading area with rounded corners
    const int panelWidth = 320;
    const int panelHeight = 55;
    const int panelX = (SCREEN_WIDTH - panelWidth) / 2;
    const int panelY = Loading::BANNER_Y + 5;
    pdcpp::Rectangle<int> loadingPanel(
        panelX, panelY,
        panelWidth, panelHeight
    );
    pdcpp::Graphics::fillRoundedRectangle(loadingPanel, 8, pdcpp::Colors::black);

    // Draw loading bar frame (square corners - too small for rounded corners to work properly)
    pdcpp::Rectangle<int> progressBarFrame(
        Loading::PROGRESS_BAR_X, Loading::PROGRESS_BAR_Y,
        Loading::PROGRESS_BAR_WIDTH, Loading::PROGRESS_BAR_HEIGHT
    );
    pdcpp::Graphics::drawRectangle(progressBarFrame, pdcpp::Colors::white);

    // Draw loading progress fill (with padding, square corners)
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

    // Draw Max Score using Font::drawWrappedText for automatic centering
    pdcpp::Rectangle<float> maxLocalScoreBounds(
        MainMenu::MAX_LOCAL_SCORE_X, MainMenu::MAX_LOCAL_SCORE_Y,
        SCREEN_WIDTH, static_cast<float>(font.getFontHeight())
    );
    SetTextDrawMode(Theme::TEXT_COLOR);
    font.drawWrappedText(
    "Max\nLocal\nKills:\n" + std::to_string(*maxScorePtr),
    maxLocalScoreBounds,
    pdcpp::Font::Center,
    pdcpp::Font::Top);
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
    RefreshSkillIcons(player);
    const int iconCount = static_cast<int>(magicIcons.size());
    if (iconCount > 0)
    {
        unsigned int activeMagic = player->GetSelectedMagic() % static_cast<unsigned int>(iconCount);
        const int prevIndex = (activeMagic == 0) ? iconCount - 1 : static_cast<int>(activeMagic - 1);
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
    }

    // Draw player face icon
    pd->graphics->drawBitmap(
        playerFace,
        offset.x + GameHUD::PLAYER_FACE_X_OFFSET,
        offset.y + GameHUD::PLAYER_FACE_Y_OFFSET,
        kBitmapUnflipped
    );
}

void UI::RefreshSkillIcons(const std::shared_ptr<Player>& player) const
{
    if (!player)
    {
        return;
    }

    const size_t skillCount = player->GetSkillCount();
    if (skillCount == 0)
    {
        magicIcons.clear();
        magicIconPaths.clear();
        return;
    }

    bool needsRefresh = magicIconPaths.size() != skillCount;
    if (!needsRefresh)
    {
        for (size_t i = 0; i < skillCount; ++i)
        {
            if (magicIconPaths[i] != player->GetSkillIconPath(static_cast<unsigned int>(i)))
            {
                needsRefresh = true;
                break;
            }
        }
    }

    if (!needsRefresh)
    {
        return;
    }

    magicIcons.clear();
    magicIconPaths.clear();
    magicIcons.reserve(skillCount);
    magicIconPaths.reserve(skillCount);

    const char* err = nullptr;
    for (size_t i = 0; i < skillCount; ++i)
    {
        const std::string& path = player->GetSkillIconPath(static_cast<unsigned int>(i));
        LCDBitmap* icon = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(path.c_str(), &err);
        if (icon == nullptr)
        {
            Log::Error("Error loading skill icon %s: %s", path.c_str(), err);
        }
        magicIcons.push_back(icon);
        magicIconPaths.push_back(path);
    }
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

    // Draw game over black panel
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
    pdcpp::GlobalPlaydateAPI::get()->system->removeAllMenuItems();
    currentScreen = newScreen;

    // Hide level-up popup when switching screens
    if (newScreen != GameScreen::GAME)
    {
        showLevelUpPopup = false;
        levelUpSelectedOption = 0;
        pendingLevelUps = 0;
    }
}

void UI::UpdateLoadingProgress(float progress)
{
    loadingProgress = progress;
    if (loadingProgress > 1.0f) loadingProgress = 1.0f;
    if (loadingProgress < 0.0f) loadingProgress = 0.0f;
}

void UI::UpdateStatsMenuItem(const std::shared_ptr<Player>& player)
{
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();

    // Only update stats if we're in game modes
    // Remove old menu item if it exists
    if (statsMenuItem)
    {
        pd->system->removeMenuItem(statsMenuItem);
        statsMenuItem = nullptr;
    }
    if (saveMenuItem)
    {
        pd->system->removeMenuItem(saveMenuItem);
        saveMenuItem = nullptr;
    }
    if (currentScreen != GameScreen::GAME || !player)
    {
        return;
    }

    // Format the stats strings with static storage
    static char hp[32];
    static char kills[32];
    static char time[32];
    snprintf(hp, sizeof(hp), "HP: %.0f", player->GetHP());
    snprintf(kills, sizeof(kills), "Kills: %u", player->GetMonstersKilled());
    snprintf(time, sizeof(time), "Time: %.0us", player->GetSurvivalTimeSeconds());
    const char* stats_options[] = {hp, kills, time};
    statsMenuItem = pd->system->addOptionsMenuItem(
        "Stats",
        stats_options, 3,
        nullptr,
        nullptr);
    saveMenuItem = pdcpp::GlobalPlaydateAPI::get()->system->addMenuItem("Save & Exit", SaveGameCallback, this);
}

// Static callback functions for menu items
void UI::SaveGameCallback(void* userdata)
{
    UI* ui = static_cast<UI*>(userdata);
    if (ui && ui->saveGameCallback)
    {
        ui->saveGameCallback();
        ui->gameOverCallback();
        ui->SwitchScreen(GameScreen::MAIN_MENU);
        pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
    }
}

void UI::DrawLevelUpPopup() const
{
    using namespace UIConstants;
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();

    std::shared_ptr<Player> player = entityManager->GetPlayer();
    if (!player)
    {
        return;
    }

    // Draw semi-transparent outer panel like game over screen
    pdcpp::Rectangle<int> panelRectA(
        offset.x + GameOver::PANELA_OFFSET_X,
        offset.y + GameOver::PANELA_OFFSET_Y,
        GameOver::PANELA_WIDTH,
        GameOver::PANELA_HEIGHT
    );
    pdcpp::Graphics::fillRoundedRectangle(panelRectA, 8, pdcpp::Colors::transparent50GrayB);

    // Draw main popup panel in center (larger than game over to fit content)
    const int panelWidth = 320;
    const int panelHeight = 220;
    const int panelX = offset.x - (panelWidth / 2);
    const int panelY = offset.y - (panelHeight / 2);

    pdcpp::Rectangle<int> panel(panelX, panelY, panelWidth, panelHeight);
    pdcpp::Graphics::fillRoundedRectangle(panel, 8, pdcpp::Colors::black);

    // Draw title
    const int titleY = panelY + 10;
    pdcpp::Rectangle<float> titleBounds(
        static_cast<float>(panelX), static_cast<float>(titleY),
        static_cast<float>(panelWidth), static_cast<float>(font.getFontHeight())
    );
    SetTextDrawMode(Theme::TEXT_COLOR);
    font.drawWrappedText("LEVEL UP!", titleBounds, pdcpp::Font::Center, pdcpp::Font::Top);
    pd->graphics->setDrawMode(kDrawModeCopy);

    // Draw current stats and level info
    char statsBuffer[128];
    snprintf(statsBuffer, sizeof(statsBuffer),
        "Level %u | Skill Points: %u\nSTR:%d  AGI:%d  CON:%d  HP:%.0f/%.0f",
        player->GetLevel(), player->GetSkillPoints(),
        player->GetStrength(), player->GetAgility(), player->GetConstitution(),
        player->GetHP(), player->GetMaxHP());
    const int statsY = titleY + font.getFontHeight() + 6;
    pdcpp::Rectangle<float> statsBounds(
        static_cast<float>(panelX + 10), static_cast<float>(statsY),
        static_cast<float>(panelWidth - 20), static_cast<float>(font.getFontHeight() * 3)
    );
    SetTextDrawMode(Theme::TEXT_COLOR);
    font.drawWrappedText(statsBuffer, statsBounds, pdcpp::Font::Center, pdcpp::Font::Top);
    pd->graphics->setDrawMode(kDrawModeCopy);

    // Draw stat options
    const char* statOptions[] = {
        "Strength +1",
        "Agility +1",
        "Constitution +1",
        "Max HP +5"
    };
    const int optionStartY = statsY + (font.getFontHeight() * 2) + 12;
    const int optionSpacing = 21;

    for (int i = 0; i < 4; i++)
    {
        int optionY = optionStartY + (i * optionSpacing);

        // Draw selection background for selected option
        if (i == levelUpSelectedOption)
        {
            pdcpp::Rectangle<int> selectionBox(
                panelX + 15, optionY - 2,
                panelWidth - 30, font.getFontHeight() + 4
            );
            pdcpp::Graphics::fillRoundedRectangle(selectionBox, 4, pdcpp::Colors::white);

            // Draw option text in black on white background
            pdcpp::Rectangle<float> optionBounds(
                static_cast<float>(panelX + 25), static_cast<float>(optionY),
                static_cast<float>(panelWidth - 50), static_cast<float>(font.getFontHeight())
            );
            pd->graphics->setDrawMode(kDrawModeCopy);
            font.drawWrappedText(statOptions[i], optionBounds, pdcpp::Font::Left, pdcpp::Font::Top);
        }
        else
        {
            // Draw option text in white for unselected options
            pdcpp::Rectangle<float> optionBounds(
                static_cast<float>(panelX + 25), static_cast<float>(optionY),
                static_cast<float>(panelWidth - 50), static_cast<float>(font.getFontHeight())
            );
            SetTextDrawMode(Theme::TEXT_COLOR);
            font.drawWrappedText(statOptions[i], optionBounds, pdcpp::Font::Left, pdcpp::Font::Top);
            pd->graphics->setDrawMode(kDrawModeCopy);
        }
    }

    // Draw instruction text at bottom
    const int instructionY = panelY + panelHeight - font.getFontHeight() - 8;
    pdcpp::Rectangle<float> instructionBounds(
        static_cast<float>(panelX), static_cast<float>(instructionY),
        static_cast<float>(panelWidth), static_cast<float>(font.getFontHeight())
    );
    SetTextDrawMode(Theme::TEXT_COLOR);
    font.drawWrappedText("Up/Down: Select  |  A: Confirm", instructionBounds, pdcpp::Font::Center, pdcpp::Font::Top);
    pd->graphics->setDrawMode(kDrawModeCopy);
}

// Helper methods implementation
void UI::SetTextDrawMode(LCDColor color) const
{
    if (color == kColorWhite)
        pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode(kDrawModeFillWhite);
    else
        pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode(kDrawModeCopy);
}
