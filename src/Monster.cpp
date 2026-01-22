//
// Created by Sergio Prada on 11/06/25.
//

#include "Monster.h"
#include "Globals.h"
#include "Player.h"
#include "Log.h"
#include "EnemyProjectile.h"
#include "Area.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include <algorithm>
#include <cmath>
#include <memory>


Monster::Monster(unsigned int _id, const std::string& _name, const std::string& _image, float _maxHp, int _strength, int _agility,
                 int _constitution, float _evasion, unsigned int _xp, int weapon, int armor) // Keeping weapon and armor on zeros to reduce json token utilization.
    : Creature(_id, _name, _image, _maxHp, _strength, _agility, _constitution, 0, _xp, 0, 0)
{}
void Monster::Tick(Player* player, Area* area)
{
    pdcpp::Point<int> playerTiledPosition = player->GetTiledPosition();
    switch (movementType)
    {
        case MovementType::AStar:
            HandleAStarMovement(playerTiledPosition, area);
            break;
        case MovementType::NoClip:
            HandleNoClipMovement(playerTiledPosition);
            break;
        case MovementType::Stationary:
            pathFound = false;
            path.clear();
            // Stationary monsters can fire ranged attacks
            if (CanRangedAttack(player))
            {
                FireRangedAttack(player, area);
            }
            break;
        case MovementType::RangedKite:
            HandleRangedKiteMovement(playerTiledPosition, area);
            // RangedKite monsters can fire ranged attacks
            if (CanRangedAttack(player))
            {
                FireRangedAttack(player, area);
            }
            break;
    }
    if (ShouldAttack(playerTiledPosition))
    {
        player->Damage(static_cast<float>(GetStrength()));
    }
}

std::shared_ptr<void> Monster::DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager)
{
    std::vector<Monster> creatures_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type != JSMN_OBJECT) continue;

        unsigned int decodedId; std::string decodedName; std::string decodedPath; float decodedMaxHp; int decodedStrength;
        int decodedAgility; int decodedConstitution; unsigned int decodedXp;
        MovementType decodedMovement = MovementType::AStar;

        const char* objects[] = {"id", "name", "image", "hp", "str", "agi", "con", "xp", "movement"};
        for (const char* & object : objects)
        {
            // doing (tokens[i].size*2) because the object size returns the number of elements inside.
            // for example:
            // { "id": 1, "name": "Sergio" } its size = 2. But the tokens are 4.
            std::string value = Utils::ValueDecoder(buffer, tokens, i, i+(tokens[i].size*2), object);

            if(strcmp(object, "id") == 0) decodedId = std::stoi(value);
            else if(strcmp(object, "name") == 0) decodedName = value;
            else if(strcmp(object, "image") == 0) decodedPath = value;
            else if(strcmp(object, "hp") == 0) decodedMaxHp = std::stof(value);
            else if(strcmp(object, "str") == 0) decodedStrength = std::stoi(value);
            else if(strcmp(object, "agi") == 0) decodedAgility = std::stoi(value);
            else if(strcmp(object, "con") == 0) decodedConstitution = std::stoi(value);
            else if(strcmp(object, "xp") == 0) decodedXp = std::stoi(value);
            else if(strcmp(object, "movement") == 0)
            {
                if (value == "noclip") decodedMovement = MovementType::NoClip;
                else if (value == "stationary") decodedMovement = MovementType::Stationary;
                else if (value == "ranged") decodedMovement = MovementType::RangedKite;
                else decodedMovement = MovementType::AStar;
            }
            else Log::Error("Unknown property %s", object);
        }
        creatures_decoded.emplace_back(decodedId, decodedName, decodedPath, decodedMaxHp, decodedStrength,
                                       decodedAgility, decodedConstitution, 0, decodedXp,
                                       0, 0);
        creatures_decoded.back().SetMovementType(decodedMovement);
        Log::Info("Monster ID: %d, name %s, XP: %i", decodedId, decodedName.c_str(), decodedXp);

        i+=(tokens[i].size*2);
    }
    return std::make_shared<std::vector<Monster>>(creatures_decoded);
}

void Monster::HandleAStarMovement(const pdcpp::Point<int>& playerTiledPosition, Area* area)
{
    if (pathFindingCooldown > 0)
    {
        pathFindingCooldown--;
    }
    else if (canComputePath && !pathFound && pathFindFailureCount < Globals::MAX_PATH_FIND_FAILURE_COUNT && ShouldMove(playerTiledPosition))
    {
        CalculateNodesToTarget(playerTiledPosition, area);
        pathFindingCooldown = Globals::PATH_FINDING_COOLDOWN; // Reset cooldown after pathfinding
    }
    if (pathFound)
    {
        if (!path.empty())
        {
            if (reachedNode)
            {
                nextPosition = path.back(); // Get the next position in the path
                path.pop_back(); // Remove the previous position from the path
                reachedNode = false;
            }
            Move(nextPosition, area);
        }
        else
        {
            pathFound = false; // Reset if path is empty
        }
    }
    else if (pathFindFailureCount >= Globals::MAX_PATH_FIND_FAILURE_COUNT)
    {
        pathFindFailureCount++; // Increment failure count
        if (pathFindFailureCount >= Globals::MAX_PATH_FIND_FAILURE_COUNT * Globals::MULT_FOR_RESET_PATH_FIND_FAILURE) // If too many failures, reset
        {
            pathFindFailureCount = 0; // Give the chance to try again after a long time
        }
    }
}

void Monster::HandleNoClipMovement(const pdcpp::Point<int>& playerTiledPosition)
{
    pathFound = false;
    path.clear();
    if (!ShouldMove(playerTiledPosition))
    {
        return;
    }
    MoveNoClip(playerTiledPosition);
}

void Monster::HandleRangedKiteMovement(const pdcpp::Point<int>& playerTiledPosition, Area* area)
{
    float distance = GetTiledPosition().distance(playerTiledPosition);
    if (distance > Globals::MONSTER_AWARENESS_RADIUS)
    {
        pathFound = false;
        path.clear();
        return;
    }

    bool shouldMove = false;
    pdcpp::Point<int> target = playerTiledPosition;

    if (distance < Globals::MONSTER_KITE_MIN_RANGE)
    {
        target = GetKiteTarget(playerTiledPosition, area);
        shouldMove = true;
    }
    else if (distance > Globals::MONSTER_KITE_MAX_RANGE)
    {
        shouldMove = true;
    }
    else
    {
        pathFound = false;
        path.clear();
        return;
    }

    if (target.x != lastPathTarget.x || target.y != lastPathTarget.y)
    {
        pathFound = false;
        path.clear();
        lastPathTarget = target;
    }

    if (pathFindingCooldown > 0)
    {
        pathFindingCooldown--;
    }
    else if (shouldMove && canComputePath && !pathFound && pathFindFailureCount < Globals::MAX_PATH_FIND_FAILURE_COUNT)
    {
        CalculateNodesToTarget(target, area);
        pathFindingCooldown = Globals::PATH_FINDING_COOLDOWN;
    }

    if (pathFound)
    {
        if (!path.empty())
        {
            if (reachedNode)
            {
                nextPosition = path.back();
                path.pop_back();
                reachedNode = false;
            }
            Move(nextPosition, area);
        }
        else
        {
            pathFound = false;
        }
    }
}

bool Monster::ShouldMove(pdcpp::Point<int> playerPosition) const
{
    float distance = GetTiledPosition().distance(playerPosition);
    if (distance <= Globals::MONSTER_AWARENESS_RADIUS && distance > Globals::MONSTER_ATTACK_RANGE)
    {
        return true;
    }
    return false;
}

void Monster::CalculateNodesToTarget(const pdcpp::Point<int> target, const Area* area)
{
    path.clear();
    pathFound = area->GetCollider()->ComputePath(GetTiledPosition(), target, path, MapCollision::MOVE_NORMAL, MapCollision::DEFAULT_PATH_LIMIT);
    if (!pathFound)
    {
        pathFindFailureCount++;
    }
    else
    {
        pathFindFailureCount = 0; // Reset failure count on success
    }
}

void Monster::Move(pdcpp::Point<int> target, Area* area)
{
    // We are moving in pixel coordinates to improve the movement smoothness.
    const pdcpp::Point<int> iPosCache = {GetPosition().x, GetPosition().y};
    const pdcpp::Point<int> iPos = {iPosCache.x, iPosCache.y};
    const pdcpp::Point<int> fPos = {target.x*Globals::MAP_TILE_SIZE,target.y*Globals::MAP_TILE_SIZE};
    const pdcpp::Point<int> d = {(fPos.x - iPos.x),(fPos.y - iPos.y)}; //direction vector
    pdcpp::Point<int> dn {0,0}; //direction normalized

    // normalize the direction vector
    if (d.x != 0 || d.y != 0)
    {
        /*
         *The idea here was to normalize the direction vector so that the monster moves in a straight line towards the target.
         *Nonetheless, it didn't work because when casting float to int, it would round down to 0.
         *And when scaling up the value, there's a risk to enter in an cycle where it jumps back and fort
        float length = sqrtf((d.x * d.x) + (d.y * d.y));
        if (length > 0)
        {
            d.x = d.x / length;
            d.y = d.y / length;
        }*/

        /*
         *The idea here is to scale down the movement depending on the dx.
         *The problem is that the monsters will always move at a different speed rate.
         *And they should always move at the same speed rate.
        if (GetMovementScale() > 1) Log::Error("Movement scale is greater than 1, this will cause the monster to jump beyond the nodes and move in cycle.");
        else if (GetMovementScale() <= 0) Log::Error("Movement scale is less than or equal to 0, this will cause the monster to not move.");
        else
        {
            d.x = d.x * GetMovementScale();
            d.y = d.y * GetMovementScale();
            if (d.x > 0 && d.x < 1) d.x = 1; // Ensure minimum movement in x direction is 1 unit or zero.
            if (d.y > 0 && d.y < 1) d.y = 1; // Ensure minimum movement in y direction is 1 unit or zero.
        }
        */

        /*
         *This kind of works, but still the speed relies on the nodes distance. I want speed to be constant.
        float length = sqrtf((d.x * d.x) + (d.y * d.y));
        if (length > 0)
        {
            dn.x = d.x / length;
            dn.y = d.y / length;
        }
        dn.x *= GetMovementScale();
        dn.y *= GetMovementScale();
        if (dn.x > d.x) dn.x = d.x; // Ensure we do not overshoot the target in x direction
        if (dn.y > d.y) dn.y = d.y; // Ensure we do not overshoot the target in y direction
        if (dn.x > 0 && dn.x < 1) dn.x = 1; // Ensure minimum movement in x direction is 1 unit or zero.
        if (dn.y > 0 && dn.y < 1) dn.y = 1; // Ensure minimum movement in y direction is 1 unit or zero.
        */

        int moveSpeed = GetMovementSpeed();

        // Apply slowdown ability - this is a special ability with a 10-second cooldown
        // When activated (player goes idle and cooldown is ready), enemies start at 10% speed
        // and progressively speed back up to 100% over 2.5 seconds
        if (area->IsSlowdownActive())
        {
            float idleTime = area->GetPlayerIdleTime();
            float slowdownFactor;

            if (idleTime < 0.5f)
            {
                // Initial slowdown period - enemies at 10% speed
                slowdownFactor = 0.1f;
            }
            else if (idleTime < 2.5f)
            {
                // Progressive speed-up from 10% to 100% over 2 seconds
                float progress = (idleTime - 0.5f) / 2.0f; // 0.0 to 1.0
                slowdownFactor = 0.1f + (progress * 0.9f); // 10% to 100%
            }
            else
            {
                // Back to normal speed after 2.5 seconds
                slowdownFactor = 1.0f;
            }

            moveSpeed = std::max(1, static_cast<int>(static_cast<float>(moveSpeed) * slowdownFactor));
        }

        if (d.x != 0 && d.y != 0)
        {
            // ensure move scale is less when moving in diagonals but never less than 1
            // La hipotenusa, que es la diagonal del cuadrado, es en realidad √2 veces la longitud de uno de sus lados.
            // Esto equivale aproximadamente a 1.414 veces la longitud del lado, lo que representa un aumento de alrededor del 41.4%,
            moveSpeed = std::max(1, static_cast<int>(static_cast<float>(moveSpeed) * Globals::MONSTER_DIAGONAL_MOVE_SCALE));
        }
        if (d.x > 0)
        {
            dn.x = moveSpeed;
            if (dn.x > d.x) dn.x = d.x; // Ensure we do not overshoot the target in x direction
        }
        else if (d.x < 0)
        {
            dn.x = -moveSpeed; // Ensure we do not overshoot the target in x direction
        }
        if (d.y > 0)
        {
            dn.y = moveSpeed;
            if (dn.y > d.y) dn.y = d.y; // Ensure we do not overshoot the target in y direction
        }
        else if (d.y < 0)
        {
            dn.y = -moveSpeed;
            if (dn.y < d.y) dn.y = d.y; // Ensure we do not overshoot the target in y direction
        }
    }
    else
    {
        reachedNode = true;
        return; // Already at the target position
    }
    // print dx and dy for debugging
    //Log::Info("dx (%f), dy  (%f)", d.x, d.y);

    pdcpp::Point<int> newPosition {iPos.x + dn.x, iPos.y + dn.y};
    if (area->GetCollider()->IsTileBlockedByChar(newPosition.x / Globals::MAP_TILE_SIZE, newPosition.y / Globals::MAP_TILE_SIZE))
    {
        // This is a workaround to avoid the monster getting stuck in a tile,
        // and sharing the same position as the other monsters.
        // It doesn't look smooth, but it has a certain nice feeling.
        pdcpp::Random random {};
        newPosition.x += static_cast<int>(random.next()% Globals::MONSTER_RANDOM_SPACING) - static_cast<int>((random.next()%Globals::MONSTER_RANDOM_SPACING));
        newPosition.y += static_cast<int>(random.next()% Globals::MONSTER_RANDOM_SPACING) - static_cast<int>((random.next()%Globals::MONSTER_RANDOM_SPACING));
    }
    SetPosition(newPosition);

    // calculate the actual difference between start and after movement to validate it is not stuck
    if (iPosCache.x == newPosition.x && iPosCache.y == newPosition.y)
    {
        // We force calculating a new path
        pathFound = false;
        return;
    }

    // In case the new position is close to the target, return true.
    if (newPosition.distance(target) < 1.f)
    {
        reachedNode = true;
    }

    // Calculate if the new position is beyond the target position, so we mark it as reached. And move to the next node.
    // This is good to avoid "jittering", returning when passing the node, or reducing the movement rate
    // Check if we've passed the target position in either axis and mark node as reached
    if ((dn.x > 0 && newPosition.x >= fPos.x) ||
        (dn.x < 0 && newPosition.x <= fPos.x) ||
        (dn.y > 0 && newPosition.y >= fPos.y) ||
        (dn.y < 0 && newPosition.y <= fPos.y))
    {
        reachedNode = true;
    }
}

void Monster::MoveNoClip(pdcpp::Point<int> target)
{
    const pdcpp::Point<int> iPos = {GetPosition().x, GetPosition().y};
    const pdcpp::Point<int> fPos = {target.x * Globals::MAP_TILE_SIZE, target.y * Globals::MAP_TILE_SIZE};
    const pdcpp::Point<int> d = {(fPos.x - iPos.x), (fPos.y - iPos.y)};

    if (d.x == 0 && d.y == 0)
    {
        return;
    }

    float length = sqrtf(static_cast<float>(d.x * d.x + d.y * d.y));
    if (length <= 0.0f)
    {
        return;
    }

    int moveSpeed = GetMovementSpeed();
    if (d.x != 0 && d.y != 0)
    {
        moveSpeed = std::max(1, static_cast<int>(static_cast<float>(moveSpeed) * Globals::MONSTER_DIAGONAL_MOVE_SCALE));
    }

    pdcpp::Point<int> dn {static_cast<int>((static_cast<float>(d.x) / length) * static_cast<float>(moveSpeed)),
                          static_cast<int>((static_cast<float>(d.y) / length) * static_cast<float>(moveSpeed))};
    if (dn.x == 0 && d.x != 0) dn.x = (d.x > 0) ? 1 : -1;
    if (dn.y == 0 && d.y != 0) dn.y = (d.y > 0) ? 1 : -1;

    SetPosition({iPos.x + dn.x, iPos.y + dn.y});
}

pdcpp::Point<int> Monster::GetKiteTarget(const pdcpp::Point<int>& playerTiledPosition, const Area* area) const
{
    const pdcpp::Point<int> monsterTiled = GetTiledPosition();
    int dx = monsterTiled.x - playerTiledPosition.x;
    int dy = monsterTiled.y - playerTiledPosition.y;

    if (dx == 0 && dy == 0)
    {
        dx = 1;
    }

    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

    int targetX = monsterTiled.x + stepX * Globals::MONSTER_KITE_STEP;
    int targetY = monsterTiled.y + stepY * Globals::MONSTER_KITE_STEP;

    targetX = std::clamp(targetX, 0, area->GetWidth() - 1);
    targetY = std::clamp(targetY, 0, area->GetHeight() - 1);
    return {targetX, targetY};
}

bool Monster::ShouldAttack(pdcpp::Point<int> target) const
{
    return GetTiledPosition().distance(target) <= Globals::MONSTER_ATTACK_RANGE;
}

bool Monster::CanRangedAttack(Player* player) const
{
    if (!player || !player->IsAlive())
    {
        return false;
    }
    
    // Check if player is in ranged attack range
    float distance = GetTiledPosition().distance(player->GetTiledPosition());
    if (distance > Globals::MONSTER_RANGED_ATTACK_RANGE)
    {
        return false;
    }
    
    // Check cooldown based on movement type
    unsigned int currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    unsigned int cooldown = (movementType == MovementType::Stationary) 
        ? Globals::MONSTER_STATIONARY_ATTACK_COOLDOWN 
        : Globals::MONSTER_RANGED_ATTACK_COOLDOWN;
    
    return (currentTime - lastAttackTime) >= cooldown;
}

float Monster::CalculateAngleToPlayer(Player* player) const
{
    if (!player)
    {
        return 0.0f;
    }
    
    pdcpp::Point<int> monsterPos = GetCenteredPosition();
    pdcpp::Point<int> playerPos = player->GetCenteredPosition();
    
    int dx = playerPos.x - monsterPos.x;
    int dy = playerPos.y - monsterPos.y;
    
    // Calculate angle using atan2 (returns angle in radians)
    return static_cast<float>(atan2(dy, dx));
}

void Monster::FireRangedAttack(Player* player, Area* area)
{
    if (!CanRangedAttack(player) || !area)
    {
        return;
    }
    
    unsigned int currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    lastAttackTime = currentTime;
    
    pdcpp::Point<int> monsterPos = GetCenteredPosition();
    float baseAngle = CalculateAngleToPlayer(player);
    
    // Area will handle creating projectiles with proper shared_ptr to player
    if (movementType == MovementType::Stationary)
    {
        // Fire multiple projectiles in spread pattern
        int projectileCount = Globals::MONSTER_STATIONARY_PROJECTILE_COUNT;
        float spreadAngle = Globals::MONSTER_STATIONARY_SPREAD_ANGLE;
        float angleStep = spreadAngle / static_cast<float>(projectileCount - 1);
        float startAngle = baseAngle - (spreadAngle / 2.0f);
        
        for (int i = 0; i < projectileCount; i++)
        {
            float angle = startAngle + (angleStep * static_cast<float>(i));
            area->CreateEnemyProjectile(
                monsterPos,
                angle,
                Globals::MONSTER_PROJECTILE_SPEED,
                Globals::MONSTER_PROJECTILE_SIZE,
                Globals::MONSTER_STATIONARY_PROJECTILE_DAMAGE
            );
        }
    }
    else if (movementType == MovementType::RangedKite)
    {
        // Fire single projectile
        area->CreateEnemyProjectile(
            monsterPos,
            baseAngle,
            Globals::MONSTER_PROJECTILE_SPEED,
            Globals::MONSTER_PROJECTILE_SIZE,
            Globals::MONSTER_PROJECTILE_DAMAGE
        );
    }
}
