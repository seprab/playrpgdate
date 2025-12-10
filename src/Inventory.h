#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include "Item.h"


class Inventory
{
private:
    std::vector<std::shared_ptr<Item>> items;

public:
    Inventory();
    Inventory(const Inventory& other);
    Inventory(Inventory&& other) noexcept;

    void Add(unsigned int itemId, EntityManager* entityManager, int count=1);
    std::shared_ptr<Item> Remove(int itemId);

    int Count(int itemId);
    [[nodiscard]] int Print(bool label = true) const;
    void Clear();
};

#endif