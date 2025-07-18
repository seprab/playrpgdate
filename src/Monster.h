//
// Created by Sergio Prada on 11/06/25.
//

#ifndef CARDOBLAST_MONSTER_H
#define CARDOBLAST_MONSTER_H

#include "Creature.h"
#include "Globals.h"
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
    void SetCanComputePath(bool value) { canComputePath = value; }

private:
    [[nodiscard]] bool ShouldMove(pdcpp::Point<int> playerPosition) const;
    [[nodiscard]] bool ShouldAttack(pdcpp::Point<int> target) const;
    void CalculateNodesToTarget(pdcpp::Point<int> target, const Area* area);
    void Move(pdcpp::Point<int> target, Area* area);
    std::vector<pdcpp::Point<int>> path;
    bool pathFound = false;
    int pathFindFailureCount = 0;
    pdcpp::Point<int> nextPosition = {0, 0}; // Next position to move to
    bool reachedNode = true; // Flag to check if the node was reached
    int pathFindingCooldown = 0; // Cooldown for pathfinding to avoid spamming
    bool canComputePath = true; // Flag to check if pathfinding is allowed
};


#endif //CARDOBLAST_MONSTER_H
