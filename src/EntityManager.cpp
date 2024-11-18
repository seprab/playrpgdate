#include <string>
#include <vector>
#include <pd_api/pd_api_file.h>
#include "EntityManager.h"
#include "Item.h"
#include "Weapon.h"
#include "Armor.h"
#include "Creature.h"
#include "Area.h"
#include "Door.h"
#include "jsmn.h"

// Initialize the instance pointer
EntityManager* EntityManager::instance = nullptr;
PlaydateAPI* EntityManager::pd = nullptr;

EntityManager::EntityManager(PlaydateAPI* api)
{
    pd = api;
}
EntityManager* EntityManager::GetInstance(PlaydateAPI* pdApi)
{
    if (instance == nullptr)
    {
        instance = new EntityManager(pdApi);
    }
    return instance;
}
EntityManager* EntityManager::GetInstance()
{
    return instance;
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
PlaydateAPI* EntityManager::GetPD() {
    return pd;
}

template <typename T>
void EntityManager::LoadJSON(const char* fileName)
{
    pd->system->logToConsole("Loading %s data from %s", EntityToString<T>().c_str(), fileName);
    auto* fileStat = new FileStat;
    pd->file->stat(fileName, fileStat);
    SDFile* file = pd->file->open(fileName, kFileRead);
    if(file==nullptr)
    {
        pd->system->logToConsole("Error opening the file %s: %s", fileName, pd->file->geterr());
        return;
    }
    //allocate memory for the buffer before reading data into it.
    void* buffer = new char[fileStat->size + 1]; // +1 for the null terminator
    int readResult = pd->file->read(file, buffer, fileStat->size);
    if(readResult < 0)
    {
        pd->system->logToConsole("Error reading the file %s: %s", fileName, pd->file->geterr());
        return;
    }
    pd->file->close(file);


    char* charBuffer = static_cast<char*>(buffer);
    jsmn_parser p;
    jsmntok_t t[128]; //* We expect no more than 128 JSON tokens *//*
    jsmn_init(&p);
    const int tokenNumber = jsmn_parse(&p, charBuffer, fileStat->size, t, 128);

    if(std::is_same_v<T, Item>)
    {
        Item dummy{};
        void* decodedJson = dummy.DecodeJson(charBuffer, t, tokenNumber);
        auto* items = static_cast<std::vector<Item>*>(decodedJson);
        for (Item item : *items)
        {
            data[item.GetID()] = &item;
        }
    }
    else if(std::is_same_v<T, Area>)
    {
        //TODO: I'm working on validating the area deserialization
        Area dummy{};
        void* decodedJson = dummy.DecodeJson(charBuffer, t, tokenNumber);
        auto* areas = static_cast<std::vector<Area>*>(decodedJson);

        //auto areas = Area::DecodeJson(charBuffer, t, r);
        //for (auto& area : *areas)
        //{
        //    data[area.GetID()] = &area;
        //}
    }
    else if(std::is_same_v<T, Weapon>)
    {
        //TODO: Not implemented
        // auto weapons = Weapon::DecodeJson(charBuffer, t, r);
        // for (auto& weapon : *weapons)
        // {
        //     data[weapon.GetID()] = &weapon;
        // }
    }
    else if(std::is_same_v<T, Armor>)
    {
        //TODO: Not implemented
        // auto armors = Armor::DecodeJson(charBuffer, t, r);
        // for (auto& armor : *armors)
        // {
        //     data[armor.GetID()] = &armor;
        // }
    }
    else if(std::is_same_v<T, Creature>)
    {
        //TODO: Not implemented
        // auto creatures = Creature::DecodeJson(charBuffer, t, r);
        // for (auto& creature : *creatures)
        // {
        //     data[creature.GetID()] = &creature;
        // }
    }
    else if(std::is_same_v<T, Door>)
    {
        Door dummy{};
        void* decodedDoors = dummy.DecodeJson(charBuffer, t, tokenNumber);
        auto* doors = static_cast<std::vector<Door>*>(decodedDoors);
        for (Door door : *doors)
        {
            data[door.GetID()] = &door;
        }
    }
}

template <typename T>
T* EntityManager::GetEntity(unsigned int id)
{
    return dynamic_cast<T*>(data.at(id));
}

template<> Item* EntityManager::GetEntity<Item>(const unsigned int id) {return dynamic_cast<Item*>(data.at(id));}
template<> Weapon* EntityManager::GetEntity<Weapon>(const unsigned int id) {return dynamic_cast<Weapon*>(data.at(id));}
template<> Armor* EntityManager::GetEntity<Armor>(const unsigned int id) {return dynamic_cast<Armor*>(data.at(id));}
template<> Creature* EntityManager::GetEntity<Creature>(const unsigned int id) {return dynamic_cast<Creature*>(data.at(id));}
template<> Area* EntityManager::GetEntity<Area>(const unsigned int id) {return dynamic_cast<Area*>(data.at(id));}
template<> Door* EntityManager::GetEntity<Door>(const unsigned int id) {return dynamic_cast<Door*>(data.at(id));}

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

