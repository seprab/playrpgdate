#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "Entity.h"

class EntityManager;

class Item : public Entity
{
private:
    std::string name;
    std::string description;

public:
    Item(std::string _id, std::string _name, std::string _description);

    std::string GetName() const;
    std::string GetDescription() const;
};

#endif