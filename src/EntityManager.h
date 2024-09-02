#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <string>
#include <map>
#include <pd_api.h>
#include "Entity.h"

class EntityManager
{
private:
    static EntityManager* instance;
    static PlaydateAPI* pd;
    std::map<unsigned int, Entity*> data;

    // Private constructor
    explicit EntityManager(PlaydateAPI* pdApi);

public:
    // Deleted copy constructor and assignment operator
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

    // Method to get the instance of the class
    static EntityManager* GetInstance();
    static EntityManager* GetInstance(PlaydateAPI* pdApi);

    ~EntityManager();

    PlaydateAPI* GetPD();

    template <typename T>
    void LoadJSON(const char* fileName);

    template <typename T>
    std::string EntityToString();

    template <typename T>
    T* GetEntity(unsigned int id);
};

#endif