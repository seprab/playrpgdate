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
#include "Utils.h"
#include "pdcpp/core/File.h"

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

    // Load the JSON files
    LoadJSON<Item>("data/items.json", 128);
    LoadJSON<Door>("data/doors.json", 64);
    LoadJSON<Weapon>("data/weapons.json", 64);
    LoadJSON<Armor>("data/armors.json", 128);
    LoadJSON<Creature>("data/creatures.json", 2300);
    LoadJSON<Area>("data/areas.json", 128);


}
EntityManager* EntityManager::GetInstance()
{
    return instance;
}
EntityManager::~EntityManager()
{
    for (std::pair<const unsigned, std::shared_ptr<void>>& entity : data)
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
    auto* fileHandle = new pdcpp::FileHandle(fileName, kFileRead);
    char *charBuffer = new char[fileHandle->getDetails().size + 1];
    fileHandle->read(charBuffer, fileHandle->getDetails().size);
    auto* parser = new jsmn_parser;
    jsmn_init(parser);
    DecodeJson<T>(parser, charBuffer, fileHandle->getDetails().size, limitOfTokens);
    delete[] charBuffer;
    delete fileHandle;
    delete parser;
}
template <typename T>
void EntityManager::DecodeJson(jsmn_parser *parser, char *charBuffer, const size_t len, int tokenLimit)
{
    auto t = new jsmntok_t[tokenLimit];
    Log::Info("Created jsmntok_t array of size %lu", (unsigned long)sizeof(t));
    int calculatedTokens = Utils::InitializeJSMN(parser, charBuffer, len, tokenLimit, t);
    Log::Info("Just initialized JSMN with %d tokens", calculatedTokens);
    T dummy{};
    std::shared_ptr<void> decodedJson = dummy.DecodeJson(charBuffer, t, calculatedTokens);
    auto items = static_cast<std::vector<T>*>(decodedJson.get());
    for (T item : *items)
    {
        data[item.GetId()] = std::make_shared<T>(item);
    }
    delete[] t;
}