#ifndef MY_PLAYDATE_TEST_GAME_MANAGER_H
#define MY_PLAYDATE_TEST_GAME_MANAGER_H

#include <string>
#include "pdcpp/pdnewlib.h"
#include "EntityManager.h"
#include "Player.h"

class GameManager {

public:
    explicit GameManager(PlaydateAPI* api);
    ~GameManager();
    void Update();

private:
    std::string fontpath;
    LCDFont* font;
    int x{0}, y{0};
    PlaydateAPI* pd;
    Player* player;
    std::pair<int,int> currentCameraOffset = {0,0};
};


#endif //MY_PLAYDATE_TEST_GAME_MANAGER_H
