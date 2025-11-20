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

UI::UI(const char *fontPath)
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

    magicCooldown = std::make_unique<CircularProgress>(0, 0, 13);
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
                std::shared_ptr<Player> player = EntityManager::GetInstance()->GetPlayer();
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
        case GameScreen::GAME:
            DrawGameScreen();
            break;
        case GameScreen::GAME_OVER:
            DrawGameOverScreen();
            break;
        default:
            break;
    }
}

void UI::DrawLoadingScreen() const
{
    //load image as a background
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawBitmap(backgroundLoader, 0, 0, kBitmapUnflipped);
    //And black banner for text
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(0, 170, 400, 50, kColorBlack);

    // Draw loading bar frame
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(50, 200, 300, 10, kColorWhite);
    // Draw loading progress
    int progressWidth = static_cast<int>(loadingProgress * 298);
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(51, 201, progressWidth, 8, kColorWhite);


    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeFillWhite ); // making text to draw in white
    int textLen = 0;
    static int frameCount = 0;
    const char* loadingText;
    frameCount++;
    // Draw loading text
    if (loadingProgress < 1.0f)
    {
        switch (frameCount / 10 % 3)
        {
            case 0: loadingText = "."; break;
            case 1: loadingText = ".."; break;
            case 2: loadingText = "..."; break;
            default: loadingText = ""; break;
        }
        textLen = static_cast<int>(strlen(loadingText));
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(loadingText, textLen, kASCIIEncoding, 200-textLen, 180);
    }
    else
    {
        switch (frameCount / 10 % 4)
        {
            case 0: loadingText = "Press any button to continue."; break;
            case 1: loadingText = "Press any button to continue.."; break;
            case 2: loadingText = "Press any button to continue..."; break;
            case 3:
            default: loadingText = ""; break;
        }
        textLen = static_cast<int>(strlen(loadingText));
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(loadingText, textLen,kASCIIEncoding, 100, 180);
    }
    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeCopy ); // returning it to default
}

void UI::DrawMainMenu() const
{
    //load image as a background
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawBitmap(backgroundLoader, 0, 0, kBitmapUnflipped);
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(80, 30, 240, 160, kColorBlack);

    // Draw title
    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeFillWhite ); // making text to draw in white
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawText("CardoBlast", strlen("CardoBlast"), kASCIIEncoding, 150, 50);
    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeCopy ); // returning it to default

    // Draw menu items
    for (int i = 0; i < menuItemCount; i++)
    {
        if (i == selectedMenuItem)
        {
            pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(90, 100 + i * 30, 140, 20, kColorWhite);
            pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(menuItems[i], strlen(menuItems[i]), kASCIIEncoding,
                                   100, 102 + i * 30);
        }
        else
        {
            pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeFillWhite ); // making text to draw in white
            pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(menuItems[i], strlen(menuItems[i]), kASCIIEncoding,
                                   100, 102 + i * 30);
            pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeCopy ); // returning it to default
        }
    }
}

void UI::DrawGameScreen() const
{
    std::shared_ptr<Player> player = EntityManager::GetInstance()->GetPlayer();
    PlaydateAPI* pdapi = pdcpp::GlobalPlaydateAPI::get();
    //load image as a background
    pdapi->graphics->drawBitmap(gameOverlay, offset.x-200, offset.y-120, kBitmapUnflipped);
    pdapi->graphics->setDrawMode( kDrawModeFillWhite ); // making text to draw in white

    // Show player coordinates in the top-right of the screen, x and y in separate rects to make it easier to read
    pdcpp::Point<int> playerTiledPos = player->GetTiledPosition();
    char posX[3], posY[3];
    snprintf(posX, sizeof(posX), "%d", playerTiledPos.x);
    snprintf(posY, sizeof(posY), "%d", playerTiledPos.y);
    // Ugly fix, so the first frame we avoid drawing the coordinates far from screen boundaries
    if (offset.x != 0 && offset.y != 0)
    {
        pdapi->graphics->drawText(posX, strlen(posX), kASCIIEncoding, offset.x + 125, offset.y - 115);
        pdapi->graphics->drawText(posY, strlen(posY), kASCIIEncoding, offset.x + 170, offset.y - 115);
    }


    pdapi->graphics->setDrawMode( kDrawModeCopy ); // returning it to default
    magicCooldown->UpdatePosition(offset.x, offset.y-106);
    magicCooldown->Draw(player->GetCooldownPercentage());

    const unsigned int activeMagic = player->GetSelectedMagic();
    const int indexA = (activeMagic == 0) ? static_cast<int>(magicIcons.size()) - 1 : static_cast<int>(activeMagic) - 1;
    const int indexB = static_cast<int>(activeMagic);
    const int indexC = static_cast<int>((activeMagic + 1) % static_cast<unsigned int>(magicIcons.size()));
    pdapi->graphics->drawBitmap(magicIcons[indexA], offset.x-57, offset.y + 87, kBitmapUnflipped);
    pdapi->graphics->drawBitmap(magicIcons[indexB], offset.x-17, offset.y + 85, kBitmapUnflipped);
    pdapi->graphics->drawBitmap(magicIcons[indexC], offset.x+23, offset.y + 87, kBitmapUnflipped);

    pdapi->graphics->drawBitmap(playerFace, offset.x-205, offset.y-130, kBitmapUnflipped);
}

void UI::DrawGameOverScreen() const
{
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(offset.x - 200, offset.y - 60, 400, 120, kColorBlack);
    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeFillWhite ); // making text to draw in white
    int textLen = 0;
    const char* gameOverTextA = "Game Over";
    const char* gameOverTextB = "Press A to return to the main menu.";

    pdcpp::Point<int> textPosition = pdcpp::Point<int>(offset.x, offset.y);
    textLen = static_cast<int>(strlen(gameOverTextA));
    textLen = pdcpp::GlobalPlaydateAPI::get()->graphics->getTextWidth(font, gameOverTextA, textLen, kASCIIEncoding, 0);
    textPosition.x -= (textLen/2);
    textPosition.y -= Globals::PLAYER_SIZE; // Move down for the second line
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(gameOverTextA, textLen, kASCIIEncoding, textPosition.x, textPosition.y);

    textPosition = pdcpp::Point<int>(offset.x, offset.y);
    textLen = static_cast<int>(strlen(gameOverTextB));
    textLen = pdcpp::GlobalPlaydateAPI::get()->graphics->getTextWidth(font, gameOverTextB, textLen, kASCIIEncoding, 0);
    textPosition.x -= (textLen / 2);
    textPosition.y += Globals::PLAYER_SIZE; // Move down for the second line
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(gameOverTextB, textLen, kASCIIEncoding, textPosition.x, textPosition.y);

    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeCopy );
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
