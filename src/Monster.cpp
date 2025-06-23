//
// Created by Sergio Prada on 11/06/25.
//

#include "Monster.h"

#include "Globals.h"
#include "Player.h"
#include "Log.h"


Monster::Monster(unsigned int _id, char* _name, char* _image, float _maxHp, int _strength, int _agility,
             int _constitution, float _evasion, unsigned int _xp, int weapon, int armor)
    : Creature(_id, _name, _image, _maxHp, _strength, _agility, _constitution, _evasion, _xp, weapon, armor)
{}
void Monster::Tick(Player* player, Area* area)
{
    pdcpp::Point<int> playerTiledPosition = player->GetTiledPosition();
    if (!pathFound && pathFindFailureCount < Globals::MAX_PATH_FIND_FAILURE_COUNT && ShouldMove(playerTiledPosition))
    {
        CalculateNodesToTarget(playerTiledPosition, area);
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
            Move(nextPosition);
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
    if (ShouldAttack(playerTiledPosition))
    {
        player->Damage(static_cast<float>(GetStrength()));
    }
    //Draw();
}

std::shared_ptr<void> Monster::DecodeJson(char *buffer, jsmntok_t *tokens, int size)
{
    std::vector<Monster> creatures_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type != JSMN_OBJECT) continue;

        unsigned int decodedId; char* decodedName; char* decodedPath; float decodedMaxHp; int decodedStrength;
        int decodedAgility; int decodedConstitution; float decodedEvasion; unsigned int decodedXp;
        int decodedWeapon; int decodedArmor;

        const char* objects[] = {"id", "name", "image", "hp", "str", "agi", "con", "evasion", "xp", "weapon", "armor"};
        for (const char* & object : objects)
        {
            // doing (tokens[i].size*2) because the object size returns the number of elements inside.
            // for example:
            // { "id": 1, "name": "Sergio" } its size = 2. But the tokens are 4.
            char* value = Utils::ValueDecoder(buffer, tokens, i, i+(tokens[i].size*2), object);

            if(strcmp(object, "id") == 0) decodedId = static_cast<int>(strtol(value, nullptr, 10));
            else if(strcmp(object, "name") == 0) decodedName = value;
            else if(strcmp(object, "image") == 0) decodedPath = value;
            else if(strcmp(object, "hp") == 0) decodedMaxHp = strtof(value, nullptr);
            else if(strcmp(object, "str") == 0) decodedStrength = static_cast<int>(strtol(value, nullptr, 10));
            else if(strcmp(object, "agi") == 0) decodedAgility = static_cast<int>(strtol(value, nullptr, 10));
            else if(strcmp(object, "con") == 0) decodedConstitution = static_cast<int>(strtol(value, nullptr, 10));
            else if(strcmp(object, "evasion") == 0) decodedEvasion = strtof(value, nullptr);
            else if(strcmp(object, "xp") == 0) decodedXp = std::stoi(value);
            else if(strcmp(object, "weapon") == 0) decodedWeapon = static_cast<int>(strtol(value, nullptr, 10));
            else if(strcmp(object, "armor") == 0) decodedArmor = static_cast<int>(strtol(value, nullptr, 10));
            else Log::Error("Unknown property %s", object);
        }
        creatures_decoded.emplace_back(decodedId, decodedName, decodedPath, decodedMaxHp, decodedStrength,
                                       decodedAgility, decodedConstitution, decodedEvasion, decodedXp,
                                       decodedWeapon, decodedArmor);
        Log::Info("Monster ID: %d, name %s", decodedId, decodedName);

        i+=(tokens[i].size*2);
    }
    return std::make_shared<std::vector<Monster>>(creatures_decoded);
}

bool Monster::ShouldMove(pdcpp::Point<int> playerPosition) const
{
    if (GetTiledPosition().distance(playerPosition) <= Globals::MONSTER_AWARENESS_RADIUS) {
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

void Monster::Move(pdcpp::Point<int> target)
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

    const pdcpp::Point<int> newPosition {iPos.x + dn.x, iPos.y + dn.y};
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

bool Monster::ShouldAttack(pdcpp::Point<int> target) const
{
    return GetTiledPosition().distance(target) <= Globals::MONSTER_ATTACK_RANGE;
}
