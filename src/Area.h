#ifndef AREA_H
#define AREA_H

#include "Entity.h"
#include "Inventory.h"
#include "Creature.h"
#include "Dialogue.h"

class EntityManager;
class Door;
class Creature;

class Area : public Entity
{
private:
    char* name;
    Dialogue dialogue;
    Inventory items;
    std::vector<Door*> doors;
    std::vector<Creature> creatures;

public:
    Area() = default;
    Area(unsigned int _id, char* _name, Dialogue _dialogue, Inventory _items, std::vector<Creature*> _creatures);

    Dialogue GetDialogue();
    Inventory& GetItem();
    std::vector<Door*> GetDoor();
    std::vector<Creature>& GetCreature();
    void* DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;

};

#endif