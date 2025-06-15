//
// Created by Sergio Prada on 11/06/25.
//

#include "Monster.h"
#include "Player.h"
#include "Log.h"


Monster::Monster(unsigned int _id, char* _name, char* _image, float _maxHp, int _strength, int _agility,
             int _constitution, float _evasion, unsigned int _xp, int weapon, int armor)
    : Creature(_id, _name, _image, _maxHp, _strength, _agility, _constitution, _evasion, _xp, weapon, armor)
{
    SetMovementScale(1); // Set a default movement scale for monsters
    maxMovementTick *= GetMovementScale();
    movementTick = maxMovementTick; // so we set the next position accordingly in tick movement logic
    routeToNextPosition = std::vector<pdcpp::Point<int>>(maxMovementTick, pdcpp::Point<int>(0,0));
}
void Monster::Tick(Player* player, Area* area)
{
    pdcpp::Point<int> playerTiledPosition = player->GetTiledPosition();

    if (ShouldMove(playerTiledPosition))
    {
        if (!pathFound && pathFindFailureCount < maxPathFindFailureCount)
        {
            CalculateNodesToTarget(playerTiledPosition, area);
        }
        if (pathFound)
        {
            if (!path.empty())
            {
                if (movementTick>=maxMovementTick)
                {
                    nextPosition = path.back();
                    CalculatePathToNextNode(nextPosition);
                    path.pop_back();
                    movementTick = 0; // Reset movement tick
                }
                Move(routeToNextPosition[movementTick]);
                movementTick++;
            }
            else
            {
                pathFound = false; // Reset if path is empty
            }
        }
        else if (pathFindFailureCount >= maxPathFindFailureCount)
        {
            pathFindFailureCount++; // Increment failure count
            if (pathFindFailureCount >= maxPathFindFailureCount * 100)
            {
                pathFindFailureCount = 0; // Give the chance to try again after a long time
            }
        }
    }
    if (ShouldAttack(playerTiledPosition))
    {
        player->Damage(1);
    }
    Draw();
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
    if (GetTiledPosition().distance(playerPosition) <= 50) {
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

void Monster::CalculatePathToNextNode(pdcpp::Point<int> target)
{
    /*
    // The following code of code is implemented in Flare Engine, I haven't really analyzed what's for,
    // but it seems related to consider area bounds, collision with other entities and type of step.
    ///////////////////
    float dxA = static_cast<float>(target.x - GetTiledPosition().x);
    float dyA = static_cast<float>(target.y - GetTiledPosition().y);

    float x = static_cast<float>(GetTiledPosition().x);
    float y = static_cast<float>(GetTiledPosition().y);
    area->GetCollider()->Move(x, y, dxA, dyA,
        MapCollision::MOVE_NORMAL, MapCollision::ENTITY_COLLIDE_ALL);

    ///////////////////
    */
    // Below, I implement some logic to calculate the route to the next position via pixel movement.
    const int dxB = static_cast<int>(target.x) - GetTiledPosition().x;
    const int dyB = static_cast<int>(target.y) - GetTiledPosition().y;
    for (int i=0; i< routeToNextPosition.size(); i++)
    {
        routeToNextPosition[i].x = GetPosition().x + (dxB * 16 * i / maxMovementTick);
        routeToNextPosition[i].y = GetPosition().y + (dyB * 16 * i / maxMovementTick);
    }
}

void Monster::Move(pdcpp::Point<int> target)
{
    Log::Info("Monster::Move to (%d, %d)", target.x, target.y);
    SetPosition(target);
}

bool Monster::ShouldAttack(pdcpp::Point<int> target) const
{
    return GetTiledPosition().distance(target) <= 5;
}
