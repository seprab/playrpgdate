//
// Created by Sergio Prada on 11/06/25.
//

#ifndef CARDOBLAST_MONSTER_H
#define CARDOBLAST_MONSTER_H

#include "Creature.h"
#include "pdcpp/core/Timer.h"
#include "pdcpp/graphics/Point.h"

class Player;

class Monster : public Creature
{
public:
    Monster() = default;
    Monster(const Monster &other) = default;
    Monster(Monster &&other) noexcept = default;
    Monster(unsigned int _id, char* _name, char* _image, float _maxHp, int _strength, int _agility,
             int _constitution, float _evasion, unsigned int _xp, int weapon, int armor);

    void Tick(Player* player, Area* area);
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;

private:
    bool ShouldMove(pdcpp::Point<int> playerPosition) const;
    bool ShouldAttack(pdcpp::Point<int> target);
    void CalculatePath(pdcpp::Point<int> target, const Area* area);
    void Move(pdcpp::Point<int> target, Area* area);
    std::vector<pdcpp::Point<int>> path;
    bool pathFound = false;
    int pathFindFailureCount = 0;
    int tileSize = 0;
    pdcpp::Point<int> tiledPosition{0, 0}; // Position in tiles
    int movementTick = 0; // For controlling movement speed
    pdcpp::Point<int> nextPosition = {0, 0}; // Next position to move to
};


#endif //CARDOBLAST_MONSTER_H
