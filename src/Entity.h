#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "jsmn.h"

class EntityManager;


class Entity
{
private:
    unsigned int id;

public:
    explicit Entity() = default;
    explicit Entity(unsigned int _id);
    virtual ~Entity();
    [[nodiscard]] unsigned int GetID() const;
    virtual void* DecodeJson(char *buffer, jsmntok_t *tokens, int size) = 0;
};

#endif