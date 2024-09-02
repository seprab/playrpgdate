#include <vector>
#include "Item.h"
#include "EntityManager.h"
#include "Utils.h"


Item::Item(unsigned int _id, char* itemName, char* itemDescription) :
Entity(_id), name(itemName), description(itemDescription)
{
    EntityManager::GetInstance()->GetPD()->system->logToConsole("Item created with id: %d, name: %s, description: %s", _id, itemName, itemDescription);
}
const char* Item::GetName() const
{
    return name;
}
const char* Item::GetDescription() const
{
    return description;
}

void* Item::DecodeJson(char *buffer, jsmntok_t *tokens, int size)
{
    auto items_decoded = new std::vector<Item>();
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
            items_decoded->emplace_back(decodedId, decodedName, decodedDescription);
        }
    }
    return items_decoded;
}
