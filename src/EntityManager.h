#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <map>
#include <memory>
#include <pd_api.h>
#include "Entity.h"
#include "Item.h"
#include "Door.h"
#include "Inventory.h"
#include "Creature.h"



class EntityManager
{
private:
    static EntityManager* instance;
    static PlaydateAPI* pd;
    std::map<unsigned int, std::shared_ptr<Entity>> data;


public:
    explicit EntityManager(PlaydateAPI* pdApi);

    // Deleted copy constructor and assignment operator
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

    // Method to get the instance of the class
    static EntityManager* GetInstance();

    ~EntityManager();

    PlaydateAPI* GetPD();

    template <typename T>
    void LoadJSON(const char* fileName);

    template <typename T>
    std::shared_ptr<T> GetEntityCopy(unsigned int id);

    std::shared_ptr<Item> GetItem(unsigned int id);
    std::shared_ptr<Door> GetDoor(unsigned int id);
    std::shared_ptr<Creature> GetCreature(unsigned int id);
};

#endif