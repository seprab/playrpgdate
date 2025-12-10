#include <pd_api/pd_api_file.h>
#include "EntityManager.h"
#include "Door.h"
#include "Area.h"
#include "Item.h"
#include "Armor.h"
#include "Weapon.h"
#include "Monster.h"
#include "jsmn.h"
#include "Log.h"
#include "Utils.h"
#include "pdcpp/core/File.h"

EntityManager::EntityManager()
{
    // No singleton initialization needed
}
EntityManager::~EntityManager()
{
    for (std::pair<const unsigned, std::shared_ptr<void>>& entity : data)
    {
        entity.second = nullptr;
    }
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
    auto fileHandle = std::make_unique<pdcpp::FileHandle>(fileName, kFileRead);
    auto charBuffer = std::make_unique<char[]>(fileHandle->getDetails().size + 1);
    fileHandle->read(charBuffer.get(), fileHandle->getDetails().size);
    auto parser = std::make_unique<jsmn_parser>();
    jsmn_init(parser.get());
    DecodeJson<T>(parser.get(), charBuffer.get(), fileHandle->getDetails().size, limitOfTokens);
    // Automatic cleanup via unique_ptr destructors
}
template <typename T>
void EntityManager::DecodeJson(jsmn_parser *parser, char *charBuffer, const size_t len, int tokenLimit)
{
    auto t = std::make_unique<jsmntok_t[]>(tokenLimit);
    int calculatedTokens = Utils::InitializeJSMN(parser, charBuffer, len, tokenLimit, t.get());
    Log::Info("Just initialized JSMN with %d tokens", calculatedTokens);
    T dummy{};
    std::shared_ptr<void> decodedJson = dummy.DecodeJson(charBuffer, t.get(), calculatedTokens, this);
    auto items = static_cast<std::vector<T>*>(decodedJson.get());
    for (T item : *items)
    {
        data[item.GetId()] = std::make_shared<T>(item);
    }
    // Automatic cleanup via unique_ptr destructor
}
// Explicit template instantiations
template void EntityManager::LoadJSON<Area>(const char*, int);
template void EntityManager::LoadJSON<Door>(const char*, int);
template void EntityManager::LoadJSON<Item>(const char*, int);
template void EntityManager::LoadJSON<Armor>(const char*, int);
template void EntityManager::LoadJSON<Weapon>(const char*, int);
template void EntityManager::LoadJSON<Monster>(const char*, int);