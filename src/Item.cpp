#include <vector>
#include "Item.h"
#include "EntityManager.h"
#include "Utils.h"
#include "Log.h"


Item::Item(unsigned int _id, char* itemName, char* itemDescription) :
Entity(_id)
{
    SetName(itemName);
    SetDescription(itemDescription);
    Log::Info("Item created with id: %d, name: %s, description: %s", _id, itemName, itemDescription);
}
Item::Item(const Item &item)
        : Entity(item) {
}
Item::Item(Item &&item) noexcept
: Entity(item)
{

}

std::shared_ptr<void> Item::DecodeJson(char *buffer, jsmntok_t *tokens, int size)
{
    std::vector<Item> items_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type == JSMN_OBJECT)
        {
            int decodedId{};
            char* decodedName{};
            char* decodedDescription{};
            int numOfProperties = i + (tokens[i].size * 2); //*2 because of key value pairs
            while(i<numOfProperties)
            {
                i++;
                if(tokens[i].type != JSMN_STRING) continue;
                char* parseProperty = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);

                if(strcmp(parseProperty, "id") == 0) decodedId = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                else if(strcmp(parseProperty, "name") == 0) decodedName = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
                else if(strcmp(parseProperty, "description") == 0) decodedDescription = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
            };
            items_decoded.emplace_back(decodedId, decodedName, decodedDescription);
        }
    }
    return std::make_shared<std::vector<Item>>(items_decoded);
}


