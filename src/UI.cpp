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
#include "UIConstants.h"

UI::UI(const char *fontPath, EntityManager* manager)
    : entityManager(manager)
{
    const char* err;

    font = pdcpp::GlobalPlaydateAPI::get()->graphics->loadFont(fontPath, &err);
    if (font == nullptr)
        Log::Error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontPath, err);

    const char* path = "images/ui/background.png";
    backgroundLoader = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(path, &err);
    if (backgroundLoader == nullptr)
        Log::Error("%s:%i Couldn't load background image: %s", __FILE__, __LINE__, path, err);

    path = "images/ui/gameui.png";
    gameOverlay = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(path, &err);
    if (gameOverlay == nullptr)
        Log::Error("%s:%i Couldn't load background image: %s", __FILE__, __LINE__, path, err);

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
    pdcpp::GlobalPlaydateAPI::get()->graphics->setFont(font);

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
            DrawGameOverScreen();
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
    pd->graphics->fillRect(
        0, Loading::BANNER_Y,
        SCREEN_WIDTH, Loading::BANNER_HEIGHT,
        Theme::PANEL_COLOR
    );

    // Draw loading bar frame
    pd->graphics->drawRect(
        Loading::PROGRESS_BAR_X, Loading::PROGRESS_BAR_Y,
        Loading::PROGRESS_BAR_WIDTH, Loading::PROGRESS_BAR_HEIGHT,
        Theme::PROGRESS_BAR_COLOR
    );

    // Draw loading progress fill
    int progressWidth = static_cast<int>(
        loadingProgress * (Loading::PROGRESS_BAR_WIDTH - 2 * Loading::PROGRESS_BAR_PADDING)
    );
    pd->graphics->fillRect(
        Loading::PROGRESS_BAR_X + Loading::PROGRESS_BAR_PADDING,
        Loading::PROGRESS_BAR_Y + Loading::PROGRESS_BAR_PADDING,
        progressWidth,
        Loading::PROGRESS_BAR_HEIGHT - 2 * Loading::PROGRESS_BAR_PADDING,
        Theme::PROGRESS_BAR_COLOR
    );

    // Animated loading text
    static int frameCount = 0;
    const char* loadingText;
    frameCount++;

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
        DrawCenteredText(loadingText, Loading::TEXT_Y, Theme::TEXT_COLOR);
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
        int textWidth = GetTextWidth(loadingText);
        SetTextDrawMode(Theme::TEXT_COLOR);
        pd->graphics->drawText(
            loadingText, strlen(loadingText), kASCIIEncoding,
            Loading::TEXT_X_LEFT, Loading::TEXT_Y
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

    // Draw menu panel
    pd->graphics->fillRect(
        MainMenu::PANEL_X, MainMenu::PANEL_Y,
        MainMenu::PANEL_WIDTH, MainMenu::PANEL_HEIGHT,
        Theme::PANEL_COLOR
    );

    // Draw title
    const char* title = "CardoBlast";
    DrawCenteredText(title, MainMenu::TITLE_Y, Theme::TEXT_COLOR);

    // Draw menu items
    for (int i = 0; i < menuItemCount; i++)
    {
        int itemY = MainMenu::MENU_START_Y + i * MainMenu::MENU_ITEM_SPACING;
        int textY = itemY + MainMenu::MENU_TEXT_Y_OFFSET;

        if (i == selectedMenuItem)
        {
            // Draw selection highlight
            pd->graphics->fillRect(
                MainMenu::MENU_ITEM_X, itemY,
                MainMenu::MENU_ITEM_WIDTH, MainMenu::MENU_ITEM_HEIGHT,
                Theme::SELECTED_COLOR
            );
            // Draw text in inverse color (black on white)
            pd->graphics->drawText(
                menuItems[i], strlen(menuItems[i]), kASCIIEncoding,
                MainMenu::MENU_TEXT_X, textY
            );
        }
        else
        {
            // Draw unselected text
            SetTextDrawMode(Theme::TEXT_COLOR);
            pd->graphics->drawText(
                menuItems[i], strlen(menuItems[i]), kASCIIEncoding,
                MainMenu::MENU_TEXT_X, textY
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
    PlaydateAPI* pd = pdcpp::GlobalPlaydateAPI::get();

    // Draw semi-transparent panel for game over screen
    pd->graphics->fillRect(
        offset.x + GameOver::PANEL_OFFSET_X,
        offset.y + GameOver::PANEL_OFFSET_Y,
        GameOver::PANEL_WIDTH,
        GameOver::PANEL_HEIGHT,
        Theme::PANEL_COLOR
    );

    const char* gameOverText = "Game Over";
    const char* instructionText = "Press A to return to the main menu.";

    // Draw "Game Over" text (centered, above center)
    int gameOverTextWidth = GetTextWidth(gameOverText);
    int gameOverX = offset.x - (gameOverTextWidth / 2);
    int gameOverY = offset.y - GameOver::LINE_SPACING;

    SetTextDrawMode(Theme::TEXT_COLOR);
    pd->graphics->drawText(
        gameOverText, strlen(gameOverText), kASCIIEncoding,
        gameOverX, gameOverY
    );

    // Draw instruction text (centered, below center)
    int instructionTextWidth = GetTextWidth(instructionText);
    int instructionX = offset.x - (instructionTextWidth / 2);
    int instructionY = offset.y + GameOver::LINE_SPACING;

    pd->graphics->drawText(
        instructionText, strlen(instructionText), kASCIIEncoding,
        instructionX, instructionY
    );

    pd->graphics->setDrawMode(kDrawModeCopy);
}

void UI::SwitchScreen(GameScreen newScreen)
{
    currentScreen = newScreen;
}

void UI::UpdateLoadingProgress(float progress)
{
    loadingProgress = progress;
    if (loadingProgress > 1.0f) loadingProgress = 1.0f;
    if (loadingProgress < 0.0f) loadingProgress = 0.0f;
}

// Helper methods implementation
void UI::DrawCenteredText(const char* text, int y, LCDColor color) const
{
    int textWidth = GetTextWidth(text);
    int x = UIConstants::SCREEN_CENTER_X - (textWidth / 2);

    SetTextDrawMode(color);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(
        text, strlen(text), kASCIIEncoding, x, y
    );
    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode(kDrawModeCopy);
}

int UI::GetTextWidth(const char* text) const
{
    return pdcpp::GlobalPlaydateAPI::get()->graphics->getTextWidth(
        font, text, strlen(text), kASCIIEncoding, 0
    );
}

void UI::SetTextDrawMode(LCDColor color) const
{
    if (color == kColorWhite)
        pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode(kDrawModeFillWhite);
    else
        pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode(kDrawModeCopy);
}
