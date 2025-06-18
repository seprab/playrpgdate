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
    SetMovementScale(10); // Set a default movement scale for monsters
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
                if (reachedNode)
                {
                    nextPosition = path.back();
                    path.pop_back();
                    reachedNode = false;
                }
                Move(nextPosition);
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

void Monster::Move(pdcpp::Point<int> target)
{
    // We are moving in pixel coordinates to improve the movement smoothness.
    target.x *= 16;
    target.y *= 16;
    pdcpp::Point<float> d = {
        static_cast<float>(target.x - GetPosition().x),
        static_cast<float>(target.y - GetPosition().y)};

    // normalize the direction vector
    if (d.x != 0 || d.y != 0)
    {
        float length = sqrtf((d.x * d.x) + (d.y * d.y));
        if (length > 0)
        {
            d.x = d.x / length;
            d.y = d.y / length;
        }
    }
    else
    {
        reachedNode = true;
        return; // Already at the target position
    }

    // print dx and dy for debugging
    Log::Info("dx (%d), dy  (%d)", d.x, d.y);

    d.x *= GetMovementScale();
    d.y *= GetMovementScale();
    pdcpp::Point<int> newPosition {
        static_cast<int>(GetPosition().x + d.x),
        static_cast<int>(GetPosition().y + d.y)};
    SetPosition(newPosition);
    // In case the new position is close to the target, return true.
    if (GetPosition().distance(target) < 1)
    {
        reachedNode = true;
    }
}

bool Monster::ShouldAttack(pdcpp::Point<int> target) const
{
    return GetTiledPosition().distance(target) <= 5;
}
