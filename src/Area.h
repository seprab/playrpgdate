#ifndef AREA_H
#define AREA_H

#include <vector>
#include <string>
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
    Dialogue dialogue;
    Inventory items;
    std::vector<Door*> doors;
    std::vector<Creature> creatures;

public:
    Area(std::string _id, Dialogue _dialogue, Inventory _items, std::vector<Creature*> _creatures);

    Dialogue GetDialogue();
    Inventory& GetItem();
    std::vector<Door*> GetDoor();
    std::vector<Creature>& GetCreature();
};

#endif