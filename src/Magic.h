//
// Created by Sergio Prada on 26/12/24.
//

#ifndef CARDOBLAST_MAGIC_H
#define CARDOBLAST_MAGIC_H


#include "pdcpp/graphics/Point.h"
#include <memory>
#include "Area.h"
class Area;
class Player;

class Magic{
public:
    Magic() = delete;
    explicit Magic(pdcpp::Point<int> Position, std::weak_ptr<Player> player);
    bool operator==(const Magic& other) const {return this == &other;}

    void Update(const std::shared_ptr<Area>& area);
    virtual void Draw() const;
    virtual void Terminate();
    virtual void HandleInput();
    virtual void Damage(const std::shared_ptr<Area>& area);
    [[nodiscard]] bool IsAlive() const{return isAlive;}

protected:
    std::weak_ptr<Player> player;
    bool isAlive = false;
    unsigned int bornTime;
    unsigned int iLifetime{}; //milliseconds
    unsigned int elapsedTime;
    pdcpp::Point<int> position;
};


#endif //CARDOBLAST_MAGIC_H
