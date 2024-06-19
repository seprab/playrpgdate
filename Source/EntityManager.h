#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <string>
#include <map>
#include <pd_api.h>
#include "Entity.h"

class EntityManager
{
private:
    std::map<std::string, Entity*> data;
    static PlaydateAPI* pd;

public:
    explicit EntityManager(PlaydateAPI* api);
    ~EntityManager();

    template <typename T>
    void LoadJSON(const char* fileName);

    template <typename T>
    T* GetEntity(std::string id);

    template <typename T>
    std::string EntityToString();
};

#endif