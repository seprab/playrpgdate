//
// Created by Sergio Prada on 24/12/24.
//

#ifndef CARDOBLAST_UI_H
#define CARDOBLAST_UI_H

#include <functional>
#include <utility>
#include "pd_api.h"

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
    const char* menuItems[3] = {"New Game", "Load Game", "Exit"};
    int menuItemCount = 3;
    LCDFont* font;
    std::pair<int,int> offset;

    std::function<void()> newGameCallback;


public:
    explicit UI(const char*);
    void Update();
    void Draw();
    void HandleInputs();
    void SwitchScreen(GameScreen newScreen);

    void UpdateLoadingProgress(float progress);
    void SetOffset(std::pair<int,int> newOffset) { offset = newOffset; }

    void SetOnNewGameSelected(std::function<void()> callback){newGameCallback = std::move(callback);}

private:
    void DrawLoadingScreen() const;
    void DrawMainMenu();
    void DrawGameScreen();
};


#endif //CARDOBLAST_UI_H
