#ifndef ENTITY_H
#define ENTITY_H

#include <string>

class EntityManager;

class Entity
{
private:
    std::string id;

public:
    Entity(std::string _id);
    virtual ~Entity();

    std::string GetID();
};

#endif