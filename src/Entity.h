#ifndef ENTITY_H
#define ENTITY_H
#include <memory>
#include "jsmn.h"

class EntityManager;


class Entity
{
private:
    unsigned int id{};

public:
    explicit Entity() = default;
    explicit Entity(unsigned int _id);
    Entity(const Entity& other) = default;
    Entity(Entity&& other) noexcept = default;
    virtual ~Entity();
    [[nodiscard]] unsigned int GetId() const;
    virtual std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) = 0;
};

#endif