#include <utility>
#include <vector>
#include <string>
#include "Area.h"
#include "Door.h"
#include "Entity.h"
#include "Inventory.h"
#include "Dialogue.h"
#include "Utils.h"

Area::Area(unsigned int _id, char* _name, Dialogue _dialogue, Inventory _items, std::vector<Creature*> _creatures)
        : Entity(_id), name(_name), dialogue(std::move(_dialogue)), items(std::move(_items))
{
    for (auto creature : _creatures)
    {
        _creatures.push_back(creature);
    }
}

Dialogue Area::GetDialogue()
{
    return dialogue;
}

Inventory& Area::GetItem()
{
    return items;
}

std::vector<Door*> Area::GetDoor()
{
    return doors;
}

std::vector<Creature>& Area::GetCreature()
{
    return creatures;
}

void* Area::DecodeJson(char *buffer, jsmntok_t *tokens, int size) {
    auto decodedAreas = new std::vector<Area>();
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type == JSMN_OBJECT)
        {
            char* decodedName{};
            Dialogue decodedDialogue{};
            std::vector<Door*> decodedDoors{};
            Inventory decodedInventory{};
            std::vector<Creature*> decodedCreatures{};

            int endOfObject = tokens[i].end;
            while(tokens[i].end < endOfObject)
            {
                i++;
                if(tokens[i].type != JSMN_STRING) continue;
                char* parseProperty = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);

                if(strcmp(parseProperty, "name") == 0)
                {
                    decodedName = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
                }
                else if(strcmp(parseProperty, "dialogue") == 0)
                {
                    i++; //move into the dialogue token
                    char* dialogueBuffer = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);
                    decodedDialogue = Dialogue(dialogueBuffer);
                }
                else if(strcmp(parseProperty, "doors") == 0)
                {
                    i++; //move into the doors array token
                    int numOfDoors = (tokens[i].size);
                    for (int j = 0; j < numOfDoors; j++)
                    {
                        int decodedDoor = std::stoi(Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end));
                        decodedDoors.emplace_back(EntityManager::GetInstance()->GetEntity<Door>(decodedDoor));
                    }
                    i=i+numOfDoors;
                }
                else if(strcmp(parseProperty, "items") == 0)
                {
                    i++; //move into the inventory array token
                    int endOfItems = tokens[i].end;
                    while(tokens[i].end < endOfItems)
                    {
                        i++;
                        if(tokens[i].type != JSMN_ARRAY) continue;
                        int itemID = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                        int itemCount = std::stoi(Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end));
                        decodedInventory.Add(EntityManager::GetInstance()->GetEntity<Item>(itemID), itemCount);
                    }
                }
                else if(strcmp(parseProperty, "weapons") == 0)
                {
                    i++; //move into the inventory array token
                    int endOfItems = tokens[i].end;
                    while(tokens[i].end < endOfItems)
                    {
                        i++;
                        if(tokens[i].type != JSMN_ARRAY) continue;
                        int itemID = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                        int itemCount = std::stoi(Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end));
                        decodedInventory.Add(EntityManager::GetInstance()->GetEntity<Item>(itemID), itemCount);
                    }
                }
                else if(strcmp(parseProperty, "armor") == 0)
                {
                    i++; //move into the inventory array token
                    int endOfItems = tokens[i].end;
                    while(tokens[i].end < endOfItems)
                    {
                        i++;
                        if(tokens[i].type != JSMN_ARRAY) continue;
                        int itemID = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                        int itemCount = std::stoi(Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end));
                        decodedInventory.Add(EntityManager::GetInstance()->GetEntity<Item>(itemID), itemCount);
                    }
                }
                else if(strcmp(parseProperty, "creatures") == 0)
                {
                    i++; //move into the doors array token
                    int numOfCreatures = tokens[i].size;
                    for (int j = 0; j < numOfCreatures; j++)
                    {
                        int creatureId = std::stoi(Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end));
                        decodedCreatures.emplace_back(EntityManager::GetInstance()->GetEntity<Creature>(creatureId));
                    }
                    i=i+numOfCreatures;
                }
            }
            decodedAreas->emplace_back(0, decodedName, decodedDialogue, decodedInventory, decodedCreatures);
        }
    }
    return decodedAreas;
}