#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <pd_api.h>
#include "Entity.h"

class EntityManager;

class Item : public Entity
{
public:
    Item() = default;
    Item(unsigned int _id, char *itemName, char *itemDescription);
    [[nodiscard]] const char * GetName() const;
    [[nodiscard]] const char * GetDescription() const;
    void* DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;

private:
    char* name{};
    char* description{};
};

#endif