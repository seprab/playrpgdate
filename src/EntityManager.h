#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <string>
#include <map>
#include <pd_api.h>
#include "Entity.h"

class EntityManager
{
private:
    std::map<unsigned int, Entity*> data;

public:
    static PlaydateAPI* pd;

    explicit EntityManager(PlaydateAPI* api);
    ~EntityManager();

    template <typename T>
    void LoadJSON(const char* fileName);

    template <typename T>
    std::string EntityToString();

    template <typename T>
    T* GetEntity(unsigned int id);
};

#endif