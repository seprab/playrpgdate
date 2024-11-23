#include "Inventory.h"
#include "Item.h"
#include "EntityManager.h"
#include "Log.h"

Inventory::Inventory() = default;
Inventory::Inventory(const Inventory &other) {

}

Inventory::Inventory(Inventory &&other) noexcept {

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

void Inventory::Add(unsigned int itemId, int count)
{
    for (int i = 0; i < count; i++)
    {
        std::shared_ptr<void> item =  EntityManager::GetInstance()->GetEntity(itemId);

        if (item == nullptr)
        {
            Log::Error("Door with ID %d not found", itemId);
            continue;
        }
        items.push_back(std::static_pointer_cast<Item>(item));
        Log::Info("Added %s to inventory.", items.back()->GetName());
    }
}

std::shared_ptr<Item> Inventory::Remove(int itemId)
{
    int index = -1;
    for (int j=0; j<items.size(); j++)
    {
        if (items[j]->GetId() == itemId)
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
    std::shared_ptr outItem = std::move(items[index]);
    items.erase(items.begin() + index); //Validate if this is really required
    return outItem;
}

int Inventory::Count(int itemId)
{
    int count = 0;
    for (auto & item : items)
    {
        if (item->GetId() == itemId)
        {
            count++;
        }
    }
    return count;
}


