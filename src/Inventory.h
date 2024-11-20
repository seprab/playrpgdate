#ifndef INVENTORY_H
#define INVENTORY_H

#include <utility>
#include "EntityManager.h"

class Item;
class Weapon;
class Armor;

class Inventory
{
private:
    std::vector<std::unique_ptr<Item>> items;

public:
    Inventory();

    void Add(int itemId, int count=1);
    std::unique_ptr<Item> Remove(int itemId);

    int Count(int itemId);
    int Print(bool label = true);
    void Clear();
};

#endif