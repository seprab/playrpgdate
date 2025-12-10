#include <vector>
#include "Item.h"
#include "EntityManager.h"
#include "Utils.h"
#include "Log.h"


Item::Item(const unsigned int _id, char* itemName, char* itemDescription) :
Entity(_id)
{
    SetName(itemName);
    SetDescription(itemDescription);
    Log::Info("Item created with id: %d, name: %s, description: %s", _id, itemName, itemDescription);
}

std::shared_ptr<void> Item::DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager)
{
    std::vector<Item> items_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type == JSMN_OBJECT)
        {
            int decodedId{};
            char* decodedName{};
            char* decodedDescription{};

            const char* objects[] = {"id", "name", "description"};
            for (const char* & object : objects)
            {
                char* value = Utils::ValueDecoder(buffer, tokens, i, i+(tokens[i].size*2), object);
                if(strcmp(object, "id") == 0) decodedId = static_cast<int>(strtol(value, nullptr, 10));
                else if(strcmp(object, "name") == 0) decodedName = value;
                else if(strcmp(object, "description") == 0) decodedDescription = value;
            }
            items_decoded.emplace_back(decodedId, decodedName, decodedDescription);
            i+=(tokens[i].size*2);
        }
    }
    return std::make_shared<std::vector<Item>>(items_decoded);
}


