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
    if (instance!= nullptr)
    {
        pd->system->logToConsole("EntityManager was already initialized. This is a bug");
        return;
    }
    pd = api;
    pd->system->logToConsole("Initialize Entity Manager");
    instance = this;

    LoadJSON<Item>("data/items.json");
    LoadJSON<Door>("data/doors.json");
    LoadJSON<Weapon>("data/weapons.json");
    LoadJSON<Armor>("data/armors.json");
    LoadJSON<Creature>("data/creatures.json");
    LoadJSON<Area>("data/areas.json");
}
EntityManager* EntityManager::GetInstance()
{
    return instance;
}
EntityManager::~EntityManager()
{
    for (auto& entity : data)
    {
        entity.second = nullptr;
    }
}
PlaydateAPI* EntityManager::GetPD() {
    return pd;
}

template <typename T>
void EntityManager::LoadJSON(const char* fileName)
{
    pd->system->logToConsole("Loading data from %s", fileName);
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

    T dummy{};
    dummy.DecodeJson(charBuffer, t, tokenNumber);
    std::shared_ptr<void> decodedJson = dummy.DecodeJson(charBuffer, t, tokenNumber);
    auto items = static_cast<std::vector<T>*>(decodedJson.get());
    for (T item : *items)
    {
        data[item.GetId()] = std::make_shared<T>(item);
    }
}

template <typename T>
std::shared_ptr<T> EntityManager::GetEntityCopy(unsigned int id)
{
    auto it = data.find(id);
    if(it!=data.end())
    {
        return std::dynamic_pointer_cast<T>(it->second);
    }
    return nullptr;
}

//Adding the definitions below for avoiding linking errors. NEVER DELETE THIS
std::shared_ptr<Item> EntityManager::GetItem(unsigned int id){return GetEntityCopy<Item>(id);}
std::shared_ptr<Door> EntityManager::GetDoor(unsigned int id){return GetEntityCopy<Door>(id);}
std::shared_ptr<Creature> EntityManager::GetCreature(unsigned int id){return GetEntityCopy<Creature>(id);}

