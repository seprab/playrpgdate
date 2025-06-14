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
    SetMovementScale(0.1f); // Set a default movement scale for monsters
}
void Monster::Tick(Player* player, Area* area)
{
    pdcpp::Point<int> playerPosition = player->GetPosition();
    if (tileSize == 0) tileSize = area->GetTileWidth(); // Assuming square tiles
    playerPosition.x /= tileSize;
    playerPosition.y /= tileSize;
    tiledPosition.x = GetPosition().x / tileSize;
    tiledPosition.y = GetPosition().y / tileSize;

    if (ShouldMove(playerPosition))
    {
        if (!pathFound && pathFindFailureCount < 5)
        {
            CalculatePath(playerPosition, area);
        }
        else if (pathFound)
        {
            if (!path.empty())
            {
                if (movementTick>10)
                {
                    nextPosition = path.back();
                    path.pop_back();
                    movementTick = 0; // Reset movement tick
                }
                Move(nextPosition, area);
                movementTick++;
            }
            else
            {
                pathFound = false; // Reset if path is empty
            }
        }
    }
    if (ShouldAttack(playerPosition))
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
    if (tiledPosition.distance(playerPosition) <= 50) {
        return true;
    }
    return false;
}

void Monster::CalculatePath(const pdcpp::Point<int> target, const Area* area)
{
    path.clear();
    pathFound = area->GetCollider()->ComputePath(tiledPosition, target, path, MapCollision::MOVE_NORMAL, MapCollision::DEFAULT_PATH_LIMIT);
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
    float dx = static_cast<float>(target.x - tiledPosition.x) * GetMovementScale();
    float dy = static_cast<float>(target.y - tiledPosition.y) * GetMovementScale();

    float x = static_cast<float>(tiledPosition.x);
    float y = static_cast<float>(tiledPosition.y);
    area->GetCollider()->Move(x, y, dx, dy,
        MapCollision::MOVE_NORMAL, MapCollision::ENTITY_COLLIDE_ALL);
    target.x = x*tileSize;
    target.y = y*tileSize;
    SetPosition(target);
}

bool Monster::ShouldAttack(pdcpp::Point<int> target) {
    return tiledPosition.distance(target) <= 5;
}
