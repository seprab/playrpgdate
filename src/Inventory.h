#ifndef INVENTORY_H
#define INVENTORY_H

#include <list>
#include <utility>
#include <vector>
#include "EntityManager.h"

class Item;
class Weapon;
class Armor;

class Inventory
{
private:
    std::vector<Item*> items;

public:
    Inventory();

    void Add(int itemId, int count=1);
    Item* Remove(int itemId);

    int Count(int itemId);
    int Print(bool label = true);
    void Clear();
};

#endif