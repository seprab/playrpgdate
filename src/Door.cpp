#include <string>
#include <utility>

#include "Door.h"
#include "Item.h"
#include "Entity.h"
#include "EntityManager.h"

Door::Door(unsigned int _id, std::string _description, int _locked, Item* _key, std::pair<std::string, std::string> _areas)
        : Entity(_id), description(_description), locked(_locked), key(_key), areas(_areas)
{

}

std::string Door::GetDescription()
{
    return description;
}

int Door::GetLocked() const
{
    return locked;
}

void Door::SetLocked(int _locked)
{
    locked = _locked;
}

Item* Door::GetKey()
{
    return key;
}

std::pair<std::string, std::string> Door::GetArea()
{
    return areas;
}

void Door::SetArea(std::pair<std::string, std::string> _areas)
{
    areas = std::move(_areas);
}

void *Door::DecodeJson(char *buffer, jsmntok_t *tokens, int size)
{
    return nullptr;
}

