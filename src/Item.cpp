#include <utility>
#include "Item.h"
#include "EntityManager.h"
#include "Utils.h"

Item::Item(std::string _id, char* itemName, char* itemDescription) :
Entity(std::move(_id)), name(itemName), description(itemDescription)
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

std::vector<Item>* Item::DecodeJson(char *buffer, jsmntok_t *tokens, int size) {
    Utils utils;
    auto items_decoded = new std::vector<Item>();
    for (int i = 1; i < size; i++)
    {
        if (tokens[i].type == JSMN_OBJECT)
        {
            char* name = utils.Subchar(buffer, tokens[i+2].start, tokens[i+2].end);
            char* description = utils.Subchar(buffer, tokens[i+4].start, tokens[i+4].end);
            items_decoded->emplace_back("0", name, description);
            i += 4;
        }
    }
    return items_decoded;
}
