#include <iostream>
#include "Inventory.h"
#include "Item.h"

Inventory::Inventory() {

}

int Inventory::Print(bool label)
{
    if (items.empty())
    {
        EntityManager::GetInstance()->GetPD()->system->logToConsole("Inventory is empty.");
        return 0;
    }
    if (label)
    {
        EntityManager::GetInstance()->GetPD()->system->logToConsole("Inventory:");
    }
    for (auto & item : items)
    {
        EntityManager::GetInstance()->GetPD()->system->logToConsole(item->GetName());
    }
    return items.size();
}

void Inventory::Clear()
{
    items.clear();
}

void Inventory::Add(int itemId, int count)
{
    for (int i = 0; i < count; i++)
    {
        Item* itemRef = EntityManager::GetInstance()->GetEntity<Item>(itemId);
        Item* itemCopy(itemRef);
        items.push_back(itemCopy);
    }
}

Item* Inventory::Remove(int itemId)
{
    int index = -1;
    for (int j=0; j<items.size(); j++)
    {
        if (items[j]->GetID() == itemId)
        {
            index = j;
            break;
        }
    }
    if (index == -1)
    {
        EntityManager::GetInstance()->GetPD()->system->logToConsole("Item not found in inventory. Expected?");
        return nullptr;
    }
    Item* outItem = items[index];
    items.erase(items.begin() + index);
    return outItem;
}

int Inventory::Count(int itemId)
{
    int count = 0;
    for (auto & item : items)
    {
        if (item->GetID() == itemId)
        {
            count++;
        }
    }
    return count;
}
