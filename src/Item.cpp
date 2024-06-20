#include <utility>
#include "Item.h"
#include "EntityManager.h"
#include "Utils.h"
#include <vector>

Item::Item(unsigned int _id, char* itemName, char* itemDescription) :
Entity(_id), name(itemName), description(itemDescription)
{

}
const char* Item::GetName() const
{
    return name;
}
const char* Item::GetDescription() const
{
    return description;
}

void* Item::DecodeJson(char *buffer, jsmntok_t *tokens, int size) {
    auto items_decoded = new std::vector<Item>();
    for (int i = 1; i < size; i++)
    {
        if (tokens[i].type == JSMN_OBJECT)
        {
            char* name = Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end);
            char* description = Utils::Subchar(buffer, tokens[i+4].start, tokens[i+4].end);
            items_decoded->emplace_back(i, name, description);
            i += 4;
        }
    }
    return items_decoded;
}
