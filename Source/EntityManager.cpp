#include <string>
#include <map>
#include <pd_api/pd_api_file.h>
#include "EntityManager.h"
#include "Item.h"
#include "Weapon.h"
#include "Armor.h"
#include "Creature.h"
#include "Area.h"
#include "Door.h"

PlaydateAPI* EntityManager::pd = nullptr;

EntityManager::EntityManager(PlaydateAPI* api)
{
    pd = api;
    //LoadJSON<Item>("/data/items.json");
    Item item("0", api, "/data/test.json");
}

EntityManager::~EntityManager()
{
    for (auto& entity : data)
    {
        if (entity.second != nullptr)
        {
            delete entity.second;
            entity.second = nullptr;
        }
    }
}

template<typename T>
T *EntityManager::GetEntity(std::string id)
{
    if (id.substr(0, EntityToString<T>().size()) == EntityToString<T>())
        return dynamic_cast<T*>(data.at(id));
    else
        return nullptr;
}

template<typename T>
void EntityManager::LoadJSON(const char* fileName)
{
}

template <> std::string EntityManager::EntityToString<Item>() { return "item"; }
template <> std::string EntityManager::EntityToString<Weapon>() { return "weapon"; }
template <> std::string EntityManager::EntityToString<Armor>() { return "armor"; }
template <> std::string EntityManager::EntityToString<Creature>() { return "creature"; }
template <> std::string EntityManager::EntityToString<Area>() { return "area"; }
template <> std::string EntityManager::EntityToString<Door>() { return "door"; }

template void EntityManager::LoadJSON<Item>(const char*);
template void EntityManager::LoadJSON<Weapon>(const char*);
template void EntityManager::LoadJSON<Armor>(const char*);
template void EntityManager::LoadJSON<Creature>(const char*);
template void EntityManager::LoadJSON<Area>(const char*);
template void EntityManager::LoadJSON<Door>(const char*);

template Item* EntityManager::GetEntity<Item>(std::string);
template Weapon* EntityManager::GetEntity<Weapon>(std::string);
template Armor* EntityManager::GetEntity<Armor>(std::string);
template Creature* EntityManager::GetEntity<Creature>(std::string);
template Area* EntityManager::GetEntity<Area>(std::string);
template Door* EntityManager::GetEntity<Door>(std::string);
