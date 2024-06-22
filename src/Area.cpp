#include <vector>
#include <string>
#include "Area.h"
#include "Door.h"
#include "Entity.h"
#include "Inventory.h"
#include "Dialogue.h"
#include "Utils.h"

Area::Area(unsigned int _id, Dialogue _dialogue, Inventory _items, std::vector<Creature*> _creatures)
        : Entity(_id), dialogue(_dialogue), items(_items)
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
    auto items_decoded = new std::vector<Area>();
    for (int i = 1; i < size; i++)
    {
        /*if (tokens[i].type == JSMN_OBJECT)
        {
            char* decodedName = Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end);
        }
        if (tokens[i].type == JSMN_OBJECT)
        {
            //area
            char* areaName = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
            Dialogue decodedDialogue = Dialogue(Utils::Subchar(buffer, tokens[i+6].start, tokens[i+6].end), {});
            i = i+ 8;
            int endOfArray = tokens[i].end;
            while(i<endOfArray)
            {
                i++; //choices
                decodedDialogue.AddChoice(Utils::Subchar(buffer, tokens[i].start, tokens[i].end));
            }
            i++; //doors
            std::vector<char*> tempDoors {};
            endOfArray = tokens[i].end;
            while(i<endOfArray)
            {
                i++;
                tempDoors.push_back(Utils::Subchar(buffer, tokens[i].start, tokens[i].end));
            }
            Inventory decodedInventory {};
            i = i+4; //items
            endOfArray = tokens[i].end;

            *//*char* name = Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end);
            char* description = Utils::Subchar(buffer, tokens[i+4].start, tokens[i+4].end);
            items_decoded->push_back(i, name, description);
            i += 4;*//*
        }*/
    }
    return items_decoded;
}