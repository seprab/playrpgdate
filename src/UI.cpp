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
    Draw();
}

void UI::HandleInputs()
{
    if (pdcpp::GlobalPlaydateAPI::get()->system->getElapsedTime() < inputCooldown)
    {
        return; // Skip input processing if cooldown period has not passed
    }
    PDButtons current;
    pdcpp::GlobalPlaydateAPI::get()->system->getButtonState(&current, NULL, NULL);

    switch (currentScreen)
    {
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
                        // Implement load game functionality
                        break;
                }
                pdcpp::GlobalPlaydateAPI::get()->system->resetElapsedTime();
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

void UI::DrawLoadingScreen() const
{
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
        }
        textLen = strlen(loadingText);
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(loadingText, textLen, kASCIIEncoding, 200-textLen, 180);
    }
    else
    {
        switch (frameCount / 10 % 3)
        {
            case 0: loadingText = "Press any button to continue"; break;
            case 1: loadingText = "Press any button to continue"; break;
            case 2: loadingText = ""; break;
        }
        textLen = strlen(loadingText);
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawText(loadingText, textLen,kASCIIEncoding, 100, 180);
    }
    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeCopy ); // returning it to default
}

void UI::DrawMainMenu()
{
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
    // Show player coordinates in the top-right of the screen, x and y in separate rects to make it easier to read
    char resultX[4], resultY[4];
    snprintf(resultX, sizeof(resultX), "%d", offset.first);
    snprintf(resultY, sizeof(resultY), "%d", offset.second);
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(offset.first + 100, offset.second - 120, 100, 20, kColorBlack);
    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeFillWhite ); // making text to draw in white
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawTextInRect(resultX, strlen(resultX), kASCIIEncoding, offset.first + 100, offset.second - 120, 50, 15,PDTextWrappingMode::kWrapWord, PDTextAlignment::kAlignTextRight);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawTextInRect(resultY, strlen(resultY), kASCIIEncoding, offset.first + 150, offset.second - 120, 50, 15,PDTextWrappingMode::kWrapWord, PDTextAlignment::kAlignTextRight);
    pdcpp::GlobalPlaydateAPI::get()->graphics->setDrawMode( kDrawModeCopy ); // returning it to default

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
