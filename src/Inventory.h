#ifndef INVENTORY_H
#define INVENTORY_H

#include <list>
#include <utility>
#include "EntityManager.h"

class Item;
class Weapon;
class Armor;

class Inventory
{
private:
    std::list<std::pair<Item*, int>> items;

public:
    Inventory();

    void Add(Item* itemToAdd, int count);
    void Remove(Item* itemToRemove, int count);

    int Count(Item* itemToCount);
    template <typename T>
    int Count(unsigned int pos);

    template <typename T>
    T* Get(unsigned int pos);

    int Print(bool label = false);
    template <typename T>
    int Print(bool label = false);

    void Clear();

    void Merge(Inventory* inventory);
};

#endif