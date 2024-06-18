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

EntityManager::EntityManager(PlaydateAPI* api):
pd(api)
{
    LoadJSON<Item>("data/items.json");
}

EntityManager::~EntityManager() {
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
T *EntityManager::GetEntity(std::string id) {
    if (id.substr(0, EntityToString<T>().size()) == EntityToString<T>())
        return dynamic_cast<T*>(data.at(id));
    else
        return nullptr;
}

int EntityManager::readfile(void* readud, uint8_t* buf, int bufsize) {
    return pd->file->read((SDFile*)readud, buf, bufsize);
}

template<typename T>
void EntityManager::LoadJSON(std::string fileName) {
//    JsonBox::Value v;
//    v.loadFromFile(fileName);
//    JsonBox::Object obj = v.getObject();

    json_decoder decoder;
    json_reader reader;

    SDFile* file = pd->file->open(fileName.c_str(), kFileRead);
    json_value *outval;
    // pd->json->decode(&decoder, reader, outval);
    //pd->system->logToConsole(&outval);


    /*for (auto entity : obj)
    {
        std::string key = entity.first;
        data[key] = dynamic_cast<Entity*>(new T(key, entity.second, this));
    }*/
}

template <> std::string EntityManager::EntityToString<Item>() { return "item"; }
template <> std::string EntityManager::EntityToString<Weapon>() { return "weapon"; }
template <> std::string EntityManager::EntityToString<Armor>() { return "armor"; }
template <> std::string EntityManager::EntityToString<Creature>() { return "creature"; }
template <> std::string EntityManager::EntityToString<Area>() { return "area"; }
template <> std::string EntityManager::EntityToString<Door>() { return "door"; }

template void EntityManager::LoadJSON<Item>(std::string);
template void EntityManager::LoadJSON<Weapon>(std::string);
template void EntityManager::LoadJSON<Armor>(std::string);
template void EntityManager::LoadJSON<Creature>(std::string);
template void EntityManager::LoadJSON<Area>(std::string);
template void EntityManager::LoadJSON<Door>(std::string);

template Item* EntityManager::GetEntity<Item>(std::string);
template Weapon* EntityManager::GetEntity<Weapon>(std::string);
template Armor* EntityManager::GetEntity<Armor>(std::string);
template Creature* EntityManager::GetEntity<Creature>(std::string);
template Area* EntityManager::GetEntity<Area>(std::string);
template Door* EntityManager::GetEntity<Door>(std::string);
