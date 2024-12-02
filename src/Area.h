#ifndef AREA_H
#define AREA_H

#include "Entity.h"
#include "Inventory.h"
#include "Creature.h"
#include "Dialogue.h"

class EntityManager;
class Door;
class Creature;

class Area final : public Entity
{
private:
    char* name{};
    std::shared_ptr<Dialogue> dialogue;
    Inventory items;
    std::vector<std::shared_ptr<Door>> doors;
    std::vector<std::shared_ptr<Creature>> creatures;

public:
    Area() = default;
    Area(const Area& other);
    Area(Area&& other) noexcept;
    Area(unsigned int _id, char* _name, std::shared_ptr<Dialogue> _dialogue, Inventory _items, std::vector<std::shared_ptr<Creature>> _creatures);

    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;

};

#endif