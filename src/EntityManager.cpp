#include <pd_api/pd_api_file.h>
#include "EntityManager.h"
#include "Item.h"
#include "Weapon.h"
#include "Armor.h"
#include "Creature.h"
#include "Area.h"
#include "Door.h"
#include "jsmn.h"
#include "Log.h"

// Initialize the instance pointer
EntityManager* EntityManager::instance = nullptr;
PlaydateAPI* EntityManager::pd = nullptr;

EntityManager::EntityManager(PlaydateAPI* api)
{
    if (instance!= nullptr)
    {
        Log::Info("EntityManager was already initialized. This is a bug");
        return;
    }
    pd = api;
    instance = this;

    LoadJSON<Item>("data/items.json", 128);
    LoadJSON<Door>("data/doors.json", 64);
    LoadJSON<Weapon>("data/weapons.json", 64);
    LoadJSON<Armor>("data/armors.json", 128);
    LoadJSON<Creature>("data/creatures.json", 512);
    LoadJSON<Area>("data/areas.json", 128);
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

std::shared_ptr<void> EntityManager::GetEntity(unsigned int id)
{
    auto it = data.find(id);
    if(it!=data.end())
    {
        return it->second;
    }
    return nullptr;
}

/// <summary>
/// Load the JSON file and decode it into the appropriate type.
/// I have added limitOfTokens as a parameter to avoid the jsmn parser from running out of tokens.
/// The value depends on the number of elements of the JSON file.
/// I tried increasing this value automatically leading to unexpected behaviours.
/// So, It's better to manually try the best fit and adjust it from here
/// </summary>
template <typename T>
void EntityManager::LoadJSON(const char* fileName, int limitOfTokens)
{
    Log::Info("Loading data from %s", fileName);
    auto* fileStat = new FileStat;
    pd->file->stat(fileName, fileStat);
    SDFile* file = pd->file->open(fileName, kFileRead);
    if(file==nullptr)
    {
        Log::Info("Error opening the file %s: %s", fileName, pd->file->geterr());
        return;
    }
    //allocate memory for the buffer before reading data into it.
    void* buffer = new char[fileStat->size + 1]; // +1 for the null terminator
    int readResult = pd->file->read(file, buffer, fileStat->size);
    if(readResult < 0)
    {
        Log::Info("Error reading the file %s: %s", fileName, pd->file->geterr());
        return;
    }
    pd->file->close(file);

    char* charBuffer = static_cast<char*>(buffer);
    jsmn_parser parser;
    jsmn_init(&parser);
    DecodeJson<T>(&parser, charBuffer, fileStat->size, limitOfTokens);
}
template <typename T>
int EntityManager::DecodeJson(jsmn_parser *parser, char *charBuffer, const size_t len, int tokenLimit)
{
    jsmntok_t t[tokenLimit];
    int calculatedTokens = jsmn_parse(parser, charBuffer, len, t, tokenLimit);
    Log::Info("Number of tokens: %d", calculatedTokens);
    if (calculatedTokens < 0)
    {
        switch (calculatedTokens)
        {
            case jsmnerr::JSMN_ERROR_INVAL:
                Log::Error("bad token, JSON string is corrupted");
                break;
            case jsmnerr::JSMN_ERROR_NOMEM:
                Log::Error("not enough tokens, JSON string is too large");
                break;
            case jsmnerr::JSMN_ERROR_PART:
                Log::Error("JSON string is too short, expecting more JSON data");
                break;
        }
        return calculatedTokens;
    }

    T dummy{};
    std::shared_ptr<void> decodedJson = dummy.DecodeJson(charBuffer, t, calculatedTokens);
    auto items = static_cast<std::vector<T>*>(decodedJson.get());
    for (T item : *items)
    {
        data[item.GetId()] = std::make_shared<T>(item);
    }
}