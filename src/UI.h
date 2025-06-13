//
// Created by Sergio Prada on 24/12/24.
//

#ifndef CARDOBLAST_UI_H
#define CARDOBLAST_UI_H

#include <functional>
#include <utility>
#include <memory>
#include "pd_api.h"
#include "pdcpp/graphics/Point.h"
#include "CircularProgress.h"

enum class GameScreen {
    LOADING,
    MAIN_MENU,
    GAME
};

class UI{
private:
    GameScreen currentScreen;
    float loadingProgress;
    int selectedMenuItem;
    const char* menuItems[2] = {"New Game", "Load Game"};
    int menuItemCount = 2;
    LCDFont* font;
    pdcpp::Point<int> offset = {0,0};
    std::unique_ptr<CircularProgress> magicCooldown;

    const float inputCooldown{0.5f};
    LCDBitmap* backgroundLoader;
    LCDBitmap* gameOverlay;

    std::function<void()> newGameCallback;
    std::function<void()> loadGameCallback;
    std::vector<LCDBitmap*> magicIcons;
    LCDBitmap * playerFace;

public:
    explicit UI(const char*);
    void Update();
    void Draw() const;
    void HandleInputs();
    void SwitchScreen(GameScreen newScreen);

    void UpdateLoadingProgress(float progress);
    void SetOffset(pdcpp::Point<int> newOffset) { offset = newOffset; }

    void SetOnNewGameSelected(std::function<void()> callback){newGameCallback = std::move(callback);}
    void SetOnLoadGameSelected(std::function<void()> callback){loadGameCallback = std::move(callback);}

private:
    void DrawLoadingScreen() const;
    void DrawMainMenu() const;
    void DrawGameScreen() const;
};


#endif //CARDOBLAST_UI_H
