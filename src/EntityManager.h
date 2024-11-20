#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <map>
#include <pd_api.h>
#include "Entity.h"

class EntityManager
{
private:
    static EntityManager* instance;
    static PlaydateAPI* pd;
    std::map<unsigned int, std::unique_ptr<Entity>> data;


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
    std::string EntityToString();

    template <typename T>
    std::unique_ptr<T> GetEntityCopy(unsigned int id);
};

#endif