#ifndef MY_PLAYDATE_TEST_GAME_MANAGER_H
#define MY_PLAYDATE_TEST_GAME_MANAGER_H

#include "pdcpp/pdnewlib.h"
#include "EntityManager.h"
#include "Player.h"
#include "UI.h"
#include "pdcpp/graphics/Point.h"

class GameManager {

public:
    explicit GameManager(PlaydateAPI* api);
    ~GameManager();
    void Update();
    void LoadNewGame();
    void LoadSavedGame();
    void SaveGame();
    void CleanGame();

private:
    PlaydateAPI* pd;
    std::shared_ptr<Player> player;
    std::shared_ptr<UI> ui;
    std::shared_ptr<Area> activeArea;
    pdcpp::Point<int> currentCameraOffset = {0,0};
    bool isGameRunning = false;
};


#endif //MY_PLAYDATE_TEST_GAME_MANAGER_H
