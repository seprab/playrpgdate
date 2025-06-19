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
    Item(const Item &item) = default;
    Item(Item &&item) noexcept = default;

    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;

};

#endif