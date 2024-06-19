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
#include "jsmn.h"

PlaydateAPI* EntityManager::pd = nullptr;

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
    pd->system->logToConsole("Read %d bytes from the file %s", readResult, fileName);
    pd->file->close(file);
    char* charBuffer = static_cast<char*>(buffer);

    jsmn_parser p;
    jsmntok_t t[128]; //* We expect no more than 128 JSON tokens *//*
    jsmn_init(&p);
    int r = jsmn_parse(&p, charBuffer, fileStat->size, t, 128);

    for (int i = 1; i < r; i++) {
        if (jsoneq(charBuffer, &t[i], "user") == 0) {
            /* We may use strndup() to fetch string value */
            printf("- User: %.*s\n", t[i + 1].end - t[i + 1].start,
                   charBuffer + t[i + 1].start);
            i++;
        } else if (jsoneq(charBuffer, &t[i], "admin") == 0) {
            /* We may additionally check if the value is either "true" or "false" */
            printf("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
                   charBuffer + t[i + 1].start);
            i++;
        } else if (jsoneq(charBuffer, &t[i], "uid") == 0) {
            /* We may want to do strtol() here to get numeric value */
            printf("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
                   charBuffer + t[i + 1].start);
            i++;
        } else if (jsoneq(charBuffer, &t[i], "groups") == 0) {
            int j;
            printf("- Groups:\n");
            if (t[i + 1].type != JSMN_ARRAY) {
                continue; /* We expect groups to be an array of strings */
            }
            for (j = 0; j < t[i + 1].size; j++) {
                jsmntok_t *g = &t[i + j + 2];
                printf("  * %.*s\n", g->end - g->start, charBuffer + g->start);
            }
            i += t[i + 1].size + 1;
        } else {
            printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                   charBuffer + t[i].start);
        }
    }
    /*JsonBox::Value v;
    v.loadFromFile(fileName);

    JsonBox::Object obj = v.getObject();
    for (auto entity : obj)
    {
        std::string key = entity.first;
        data[key] = dynamic_cast<Entity*>(new T(key, entity.second, this));
    }*/
}
int EntityManager::jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}
template <typename T>
T* EntityManager::GetEntity(std::string id)
{
    if (id.substr(0, EntityToString<T>().size()) == EntityToString<T>())
        return dynamic_cast<T*>(data.at(id));
    else
        return nullptr;
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
