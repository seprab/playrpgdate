//
// Created by Sergio Prada on 11/06/25.
//

#ifndef CARDOBLAST_MONSTER_H
#define CARDOBLAST_MONSTER_H

#include "Creature.h"
#include "Player.h"
#include "pdcpp/graphics/Point.h"

class Player;
class Monster final : public Creature
{
public:
    void Tick(Player* player);

private:
    bool ShouldMove();
    bool ShouldAttack(pdcpp::Point<int> target);
    void CalculatePath(pdcpp::Point<int> target);
    void Move(pdcpp::Point<int> target);
};


#endif //CARDOBLAST_MONSTER_H
