#include <utility>
#include <vector>

#include "Door.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Utils.h"


Door::Door(unsigned int _id, bool _locked, int _keyId, int _areaA, int _areaB)
        : Entity(_id), locked(_locked), key(_keyId), areas(std::make_pair(_areaA, _areaB))
{

}
Door::Door(Door &other)
: Entity(other.GetId()), locked(other.GetLocked()), key(other.GetKey()), areas(other.GetAreas())
{
}
Door::Door(Door &&other) noexcept
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

int Door::GetKey()
{
    return key;
}

std::pair<int, int> Door::GetAreas()
{
    return areas;
}

std::shared_ptr<void> Door::DecodeJson(char *buffer, jsmntok_t *tokens, int size)
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

        while(i<tokens[size].end)
        {
            if (tokens[i].type != JSMN_STRING)
            {
                i++;
                continue;
            }
            char* parseProperty = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);

            if(strcmp(parseProperty, "id") == 0)
            {
                decodedId = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i++;
            }
            else if(strcmp(parseProperty, "key") == 0)
            {
                decodedKey = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i++;
            }
            else if(strcmp(parseProperty, "locked") == 0)
            {
                char* lockedChar = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
                decodedLocked = strcmp(lockedChar, "true") == 0;
                i++;
            }
            else if(strcmp(parseProperty, "areas") == 0)
            {
                decodedAreaA = std::stoi(Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end));
                decodedAreaB = std::stoi(Utils::Subchar(buffer, tokens[i+3].start, tokens[i+3].end));
                i+=3;
            }

            if (decodedId!=0 && decodedKey!=0 && decodedAreaA!=0 && decodedAreaB!=0) break;
        }
        doors_decoded.emplace_back(decodedId, decodedLocked, decodedKey, decodedAreaA, decodedAreaB);
    }
    //return std::make_shared<std::vector<Door>>(doors_decoded);
    //return doors_decoded;
    return std::static_pointer_cast<void>(std::make_shared<std::vector<Door>>(doors_decoded)); //TODO: This is failing working here
}



