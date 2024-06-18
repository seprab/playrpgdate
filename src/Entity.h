#ifndef ENTITY_H
#define ENTITY_H

#include <string>

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