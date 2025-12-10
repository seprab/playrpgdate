#include <utility>
#include <vector>
#include "Door.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Utils.h"
#include "Log.h"


Door::Door(unsigned int _id, bool _locked, int _keyId, int _areaA, int _areaB)
        : Entity(_id), locked(_locked), key(_keyId), areas(std::make_pair(_areaA, _areaB))
{

}
Door::Door(const Door &other)
: Entity(other.GetId()), locked(other.GetLocked()), key(other.GetKey()), areas(other.GetAreas())
{
}
Door::Door(const Door &&other) noexcept
:Entity(other.GetId()), locked(other.GetLocked()), key(other.GetKey()), areas(other.GetAreas())
{
}
bool Door::GetLocked() const
{
    return locked;
}

void Door::SetLocked(int _locked)
{
    locked = _locked;
}

int Door::GetKey() const
{
    return key;
}

std::pair<int, int> Door::GetAreas() const
{
    return areas;
}

std::shared_ptr<void> Door::DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager)
{
    std::vector<Door> doors_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type != JSMN_OBJECT) continue;

        unsigned int decodedId{};
        int decodedKey{};
        bool decodedLocked{};
        int decodedAreaA{};
        int decodedAreaB{};

        const char* objects[] = {"id", "area_a", "area_b", "key", "locked"};
        for (const char* & object : objects)
        {
            std::string value = Utils::ValueDecoder(buffer, tokens, i, i+(tokens[i].size*2), object);
            if(strcmp(object, "id") == 0) decodedId = std::stoi(value);
            else if(strcmp(object, "key") == 0) decodedKey = std::stoi(value);
            else if(strcmp(object, "locked") == 0) decodedLocked = (value == "true");
            else if(strcmp(object, "area_a") == 0) decodedAreaA = std::stoi(value);
            else if(strcmp(object, "area_b") == 0) decodedAreaB = std::stoi(value);
            else
            {
                Log::Error("Unknown object in Door JSON: %s", object);
                return nullptr;
            }
        }
        doors_decoded.emplace_back(decodedId, decodedLocked, decodedKey, decodedAreaA, decodedAreaB);
        Log::Info("Door ID: %d", decodedId);
        i+=(tokens[i].size*2);
    }
    return std::make_shared<std::vector<Door>>(doors_decoded);
}



