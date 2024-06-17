#ifndef MY_PLAYDATE_TEST_GAME_MANAGER_H
#define MY_PLAYDATE_TEST_GAME_MANAGER_H

#include <string>
#include <pdcpp/pdnewlib.h>

class GameManager {

public:
    explicit GameManager(PlaydateAPI* api);
    void Update();
    static constexpr int TEXT_WIDTH = 86;
    static constexpr int TEXT_HEIGHT = 16;

private:
    PlaydateAPI* pd;
    std::string fontpath;
    LCDFont* font;
    int x, y, dx, dy;

};


#endif //MY_PLAYDATE_TEST_GAME_MANAGER_H
