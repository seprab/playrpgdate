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
#include "pdcpp/graphics/Font.h"
#include "CircularProgress.h"
#include "Player.h"
#include "UIConstants.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

enum class GameScreen {
    LOADING,
    MAIN_MENU,
    GAME,
    GAME_OVER,
    PAUSE
};

class EntityManager;

class UI{
private:
    EntityManager* entityManager;
    GameScreen currentScreen;
    float loadingProgress;
    int selectedMenuItem;
    const char* menuItems[2] = {"New Game", "Load Game"};
    int menuItemCount = 2;
    pdcpp::Font font;  // Changed from LCDFont* to pdcpp::Font
    pdcpp::Point<int> offset = {0,0};
    std::unique_ptr<CircularProgress> magicCooldown;

    PDMenuItem* saveMenuItem = nullptr;
    PDMenuItem* statsMenuItem = nullptr;

    const float inputCooldown{UIConstants::Input::COOLDOWN};
    LCDBitmap* backgroundLoader;
    LCDBitmap* gameOverlay;
    LCDBitmap* pauseOverlay;

    std::function<void()> newGameCallback;
    std::function<void()> loadGameCallback;
    std::function<void()> gameOverCallback;
    std::function<void()> saveGameCallback;
    std::vector<LCDBitmap*> magicIcons;
    LCDBitmap * playerFace;

public:
    explicit UI(const char* fontPath, EntityManager* manager);
    void Update();
    void Draw() const;
    void HandleInputs();
    void SwitchScreen(GameScreen newScreen);

    void UpdateLoadingProgress(float progress);
    void UpdateStatsMenuItem(const std::shared_ptr<Player>&);
    void SetOffset(pdcpp::Point<int> newOffset) { offset = newOffset; }

    void SetOnNewGameSelected(std::function<void()> callback){newGameCallback = std::move(callback);}
    void SetOnLoadGameSelected(std::function<void()> callback){loadGameCallback = std::move(callback);}
    void SetOnGameOverSelected(std::function<void()> callback){gameOverCallback = std::move(callback);}
    void SetOnSaveGameSelected(std::function<void()> callback){saveGameCallback = std::move(callback);}

private:
    void DrawLoadingScreen() const;
    void DrawMainMenu() const;
    void DrawGameScreen() const;
    void DrawGameOverScreen() const;
    void DrawDebugInfo() const;

    // Helper methods for drawing (kept for compatibility, but prefer Font methods)
    void SetTextDrawMode(LCDColor color) const;

    // Static callback functions for menu items
    static void SaveGameCallback(void* userdata);
    static void HomeMenuCallback(void* userdata);
};


#endif //CARDOBLAST_UI_H
