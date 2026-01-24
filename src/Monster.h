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
    enum class MovementType
    {
        AStar,
        NoClip,
        Stationary,
        RangedKite
    };

    Monster() = default;
    Monster(const Monster &other) = default;
    Monster(Monster &&other) noexcept = default;
    Monster(unsigned int _id, const std::string& _name, const std::string& _image, float _maxHp, int _strength, int _agility,
             int _constitution, float _evasion, unsigned int _xp, int weapon, int armor);

    void Tick(Player* player, Area* area);
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager) override;
    void SetCanComputePath(bool value) { canComputePath = value; }
    void SetMovementType(MovementType value) { movementType = value; }
    [[nodiscard]] MovementType GetMovementType() const { return movementType; }
    
    // Ranged attack methods
    [[nodiscard]] bool CanRangedAttack(Player* player) const;
    void FireRangedAttack(Player* player, Area* area);
    [[nodiscard]] float CalculateAngleToPlayer(Player* player) const;

private:
    [[nodiscard]] bool ShouldMove(pdcpp::Point<int> playerPosition) const;
    [[nodiscard]] bool ShouldAttack(pdcpp::Point<int> target) const;
    void CalculateNodesToTarget(pdcpp::Point<int> target, const Area* area);
    void Move(pdcpp::Point<int> target, Area* area);
    void MoveNoClip(pdcpp::Point<int> target);
    void HandleAStarMovement(const pdcpp::Point<int>& playerTiledPosition, Area* area);
    void HandleNoClipMovement(const pdcpp::Point<int>& playerTiledPosition);
    void HandleRangedKiteMovement(const pdcpp::Point<int>& playerTiledPosition, Area* area);

    /**
     * @brief Calculate intelligent retreat position for kiting behavior.
     *
     * Tries to find a walkable tile away from the player by:
     * 1. Attempting direct retreat (opposite of player direction)
     * 2. Trying adjacent diagonal/cardinal directions if blocked
     * 3. Reducing distance if no valid position at full distance
     * 4. Returning current position if completely cornered
     *
     * @param playerTiledPosition Player's current tile position
     * @param area Current area (for collision checking)
     * @return Best valid retreat position, or current position if cornered
     */
    [[nodiscard]] pdcpp::Point<int> GetKiteTarget(const pdcpp::Point<int>& playerTiledPosition, const Area* area) const;
    std::vector<pdcpp::Point<int>> path;
    bool pathFound = false;
    int pathFindFailureCount = 0;
    pdcpp::Point<int> lastPathTarget = {0, 0};
    pdcpp::Point<int> nextPosition = {0, 0}; // Next position to move to
    bool reachedNode = true; // Flag to check if the node was reached
    int pathFindingCooldown = 0; // Cooldown for pathfinding to avoid spamming
    bool canComputePath = true; // Flag to check if pathfinding is allowed
    MovementType movementType = MovementType::AStar;
    unsigned int lastAttackTime = 0; // Last time the monster attacked (for cooldown)
};


#endif //CARDOBLAST_MONSTER_H
