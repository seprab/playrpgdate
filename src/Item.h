#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <pd_api/pd_api_json.h>
#include <pd_api.h>
#include "Entity.h"

class EntityManager;

class Item : public Entity
{
public:
    Item(std::string _id, char *itemName, char *itemDescription);
    [[nodiscard]] const char * GetName() const;
    [[nodiscard]] const char * GetDescription() const;
    static std::vector<Item>* DecodeJson(char *buffer, jsmntok_t *tokens, int size);

private:
    char* name{};
    char* description{};
};

#endif