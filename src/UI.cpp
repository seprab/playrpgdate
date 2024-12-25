//
// Created by Sergio Prada on 24/12/24.
//

#include "UI.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"

UI::UI(const char *fontPath)
{
    const char* err;
    font = pdcpp::GlobalPlaydateAPI::get()->graphics->loadFont(fontPath, &err);

    if (font == nullptr)
        Log::Error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontPath, err);

    currentScreen = GameScreen::LOADING;
    loadingProgress = 0.0f;
    selectedMenuItem = 0;
}

void UI::Update()
{
    pdcpp::GlobalPlaydateAPI::get()->graphics->setFont(font);

    HandleInputs();

    // Simulate loading progress
    if (currentScreen == GameScreen::LOADING)
    {
        loadingProgress += 0.02f;
        if (loadingProgress >= 1.0f)
        {
            loadingProgress = 1.0f;
        }
    }
}

void UI::HandleInputs()
{
    PDButtons current;
    pdcpp::GlobalPlaydateAPI::get()->system->getButtonState(&current, NULL, NULL);

    switch (currentScreen)
    {
        case GameScreen::LOADING:
            if (loadingProgress >= 1.0f && (current & kButtonA))
            {
                SwitchScreen(GameScreen::MAIN_MENU);
            }
            break;

        case GameScreen::MAIN_MENU:
            if (current & kButtonUp)
            {
                selectedMenuItem = (selectedMenuItem - 1 + menuItemCount) % menuItemCount;
            }
            if (current & kButtonDown)
            {
                selectedMenuItem = (selectedMenuItem + 1) % menuItemCount;
            }
            if (current & kButtonA)
            {
                switch (selectedMenuItem)
                {
                    case 0: // New Game
                        if (newGameCallback) newGameCallback();
                        SwitchScreen(GameScreen::GAME);
                        break;
                    case 1: // Load Game
                        // Implement load game functionality
                        break;
                }
            }
            break;
        default:
            break;
    }
}

void UI::Draw() {
    switch (currentScreen) {
        case GameScreen::LOADING:
            DrawLoadingScreen();
            break;
        case GameScreen::MAIN_MENU:
            DrawMainMenu();
            break;
        case GameScreen::GAME:
            DrawGameScreen();
            break;
        default:
            break;
    }
}

void UI::DrawLoadingScreen()
{
    // Draw loading bar frame
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(50, 120, 200, 20, kColorBlack);

    // Draw loading progress
    int progressWidth = static_cast<int>(loadingProgress * 198);
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(51, 121, progressWidth, 18, kColorBlack);

    // Draw loading text
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawText("Loading...", strlen("Loading..."), kASCIIEncoding, 125, 100);

    if (loadingProgress >= 1.0f)
    {
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawText("Press A to continue", strlen("Press A to continue"),kASCIIEncoding, 100, 150);
    }
}

void UI::DrawMainMenu()
{
    // Draw title
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawText("My Game", strlen("My Game"), kASCIIEncoding, 150, 50);

    // Draw menu items
    for (int i = 0; i < menuItemCount; i++)
    {
        if (i == selectedMenuItem) {
            pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(90, 100 + i * 30, 140, 20, kColorBlack);
            pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(menuItems[i], strlen(menuItems[i]), kASCIIEncoding,
                                   100, 102 + i * 30);
        } else {
            pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(menuItems[i], strlen(menuItems[i]), kASCIIEncoding,
                                   100, 102 + i * 30);
        }
    }
}
void UI::DrawGameScreen()
{
    // Show player coordinates in the top-right of the screen, x and y in separate rects to make it easier to read
    char resultX[4], resultY[4];
    snprintf(resultX, sizeof(resultX), "%d", offset.first);
    snprintf(resultY, sizeof(resultY), "%d", offset.second);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(offset.first + 100, offset.second - 120, 50, 15, kColorWhite);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(offset.first + 150, offset.second - 120, 50, 15, kColorWhite);

    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(offset.first + 102, offset.second - 118, 46, 11, kColorWhite);
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(offset.first + 152, offset.second - 118, 46, 11, kColorWhite);

    pdcpp::GlobalPlaydateAPI::get()->graphics->drawTextInRect(resultX, strlen(resultX), kASCIIEncoding, offset.first + 100, offset.second - 120, 50, 15,PDTextWrappingMode::kWrapWord, PDTextAlignment::kAlignTextRight);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawTextInRect(resultY, strlen(resultY), kASCIIEncoding, offset.first + 150, offset.second - 120, 50, 15,PDTextWrappingMode::kWrapWord, PDTextAlignment::kAlignTextRight);

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
