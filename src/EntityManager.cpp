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

PlaydateAPI* EntityManager::pd = nullptr;
static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
static char* subchar(const char* source, int start, int end);

EntityManager::EntityManager(PlaydateAPI* api)
{
    pd = api;
    LoadJSON<Item>("data/items.json");
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

template <typename T>
void EntityManager::LoadJSON(const char* fileName)
{
    auto* fileStat = new FileStat;
    pd->file->stat(fileName, fileStat);
    SDFile* file = pd->file->open(fileName, kFileRead);
    if(file==nullptr) {
        pd->system->logToConsole("Error opening the file %s: %s", fileName, pd->file->geterr());
        return;
    }
    //allocate memory for the buffer before reading data into it.
    void* buffer = new char[fileStat->size + 1]; // +1 for the null terminator
    int readResult = pd->file->read(file, buffer, fileStat->size);
    if(readResult < 0) {
        pd->system->logToConsole("Error reading the file %s: %s", fileName, pd->file->geterr());
        return;
    }
    pd->file->close(file);
    char* charBuffer = static_cast<char*>(buffer);

    jsmn_parser p;
    jsmntok_t t[128]; //* We expect no more than 128 JSON tokens *//*
    jsmn_init(&p);
    int r = jsmn_parse(&p, charBuffer, fileStat->size, t, 128);

    std::vector<T> entities_decoded = T.decodeJson(charBuffer, t, r);
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

