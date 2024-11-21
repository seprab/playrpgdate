#ifndef MY_PLAYDATE_TEST_GAME_MANAGER_H
#define MY_PLAYDATE_TEST_GAME_MANAGER_H

#include <string>
#include "pdcpp/pdnewlib.h"
#include "EntityManager.h"

class GameManager {

public:
    explicit GameManager(PlaydateAPI* api);
    ~GameManager();
    void Update();
    static constexpr int TEXT_WIDTH = 86;
    static constexpr int TEXT_HEIGHT = 16;

private:
    std::string fontpath;
    LCDFont* font;
    int x, y, dx, dy;
    PlaydateAPI* pd;
};


#endif //MY_PLAYDATE_TEST_GAME_MANAGER_H
