#ifndef ARMOR_H
#define ARMOR_H

#include <string>
#include "Item.h"

class EntityManager;

class Armor : public Item
{
private:
    int defense;

public:
    Armor(std::string _id, std::string _name, std::string _description, int _defense);

    int GetDefense();

};

#endif