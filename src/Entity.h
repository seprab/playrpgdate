#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "jsmn.h"

class EntityManager;

class Entity
{
private:
    std::string id;

public:
    explicit Entity(std::string _id);
    virtual ~Entity();

    [[nodiscard]] std::string GetID() const;
};

#endif