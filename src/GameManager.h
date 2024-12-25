#ifndef MY_PLAYDATE_TEST_GAME_MANAGER_H
#define MY_PLAYDATE_TEST_GAME_MANAGER_H

#include <string>
#include "pdcpp/pdnewlib.h"
#include "EntityManager.h"
#include "Player.h"
#include "UI.h"

class GameManager {

public:
    explicit GameManager(PlaydateAPI* api);
    ~GameManager();
    void Update();
    void LoadNewGame();
    void LoadSavedGame();

private:
    int x{0}, y{0};
    PlaydateAPI* pd;
    Player* player;
    std::shared_ptr<UI> ui;
    std::shared_ptr<Area> activeArea;
    std::pair<int,int> currentCameraOffset = {0,0};
    bool isGameRunning = false;
};


#endif //MY_PLAYDATE_TEST_GAME_MANAGER_H
