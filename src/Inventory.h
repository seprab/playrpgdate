#ifndef INVENTORY_H
#define INVENTORY_H

#include <utility>
#include <vector>
#include "item.h"


class Inventory
{
private:
    std::vector<std::shared_ptr<Item>> items;

public:
    Inventory();
    Inventory(const Inventory& other);
    Inventory(Inventory&& other) noexcept;

    void Add(unsigned int itemId, int count=1);
    std::shared_ptr<Item> Remove(int itemId);

    int Count(int itemId);
    int Print(bool label = true) const;
    void Clear();
};

#endif