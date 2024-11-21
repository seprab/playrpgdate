#include <utility>
#include "Area.h"
#include "Door.h"
#include "Entity.h"
#include "Inventory.h"
#include "Dialogue.h"
#include "Utils.h"

Area::Area(unsigned int _id, char* _name, std::shared_ptr<Dialogue> _dialogue, Inventory _items, std::vector<std::shared_ptr<Creature>> _creatures)
: Entity(_id), name(_name), dialogue(std::move(_dialogue)), items(std::move(_items))
{
    for (auto creature : _creatures)
    {
        _creatures.push_back(creature);
    }
}
Area::Area(const Area &other)
        : Entity(other.GetId()), name(other.name), dialogue(other.dialogue), items(other.items), creatures(other.creatures)
{

}
Area::Area(Area &&other) noexcept
        : Entity(other.GetId()), name(other.name), dialogue(other.dialogue), items(other.items), creatures(other.creatures)
{

}
std::shared_ptr<void> Area::DecodeJson(char *buffer, jsmntok_t *tokens, int size)
{
    std::vector<Area> decodedAreas;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type == JSMN_OBJECT)
        {
            int decodedId{};
            char* decodedName{};
            std::shared_ptr<Dialogue> decodedDialogue;
            std::vector<std::shared_ptr<Door>> decodedDoors{};
            Inventory decodedInventory{};
            std::vector<std::shared_ptr<Creature>> decodedCreatures;

            int endOfObject = tokens[i].end; //get the end of the Area object
            i++; //move into the first property of the Area object. Otherwise, the while is invalid
            while(tokens[i].end < endOfObject)
            {
                if(tokens[i].type != JSMN_STRING)
                {
                    i++;
                    continue;
                }
                char* parseProperty = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);
                if(strcmp(parseProperty, "id") == 0)
                {
                    decodedId = atoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                    i+=2; //move to the next property
                }
                else if(strcmp(parseProperty, "name") == 0)
                {
                    decodedName = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
                    i+=2; //move to the next property
                }
                else if(strcmp(parseProperty, "dialogue") == 0)
                {
                    i++; //move into the dialogue token
                    decodedDialogue = std::make_shared<Dialogue>(buffer, tokens, i);
                }
                else if(strcmp(parseProperty, "doors") == 0)
                {
                    i++; //move into the doors array token
                    int numOfDoors = tokens[i].size;
                    i++; //either move to the first element of the array or move to the next property
                    if (numOfDoors == 0) continue;
                    for (int j = 0; j < numOfDoors; j++)
                    {
                        char* door = Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end);
                        int decodedDoor = std::stoi(door);
                        decodedDoors.emplace_back(EntityManager::GetInstance()->GetEntityCopy<Door>(decodedDoor));
                    }
                    i=i+numOfDoors;
                }
                else if(strcmp(parseProperty, "items") == 0)
                {
                    i++; //move into the inventory array token
                    int endOfItems = tokens[i].end;
                    EntityManager::GetInstance()->GetPD()->system->logToConsole("Start of items:");
                    while(tokens[i].end < endOfItems)
                    {
                        i++;
                        if(tokens[i].type != JSMN_ARRAY) continue;
                        unsigned int itemID = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                        int itemCount = std::stoi(Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end));
                        EntityManager::GetInstance()->GetPD()->system->logToConsole("Item ID: %d, Item Count: %d", itemID, itemCount);
                        decodedInventory.Add(itemID, itemCount);
                    }
                }
                else if(strcmp(parseProperty, "weapons") == 0)
                {
                    i++; //move into the weapons array token
                    int endOfItems = tokens[i].end;
                    while(tokens[i].end < endOfItems)
                    {
                        i++;
                        if(tokens[i].type != JSMN_ARRAY) continue;
                        int itemID = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                        int itemCount = std::stoi(Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end));
                        decodedInventory.Add(itemID, itemCount);
                    }
                }
                else if(strcmp(parseProperty, "armor") == 0)
                {
                    i++; //move into the armor array token
                    int endOfItems = tokens[i].end;
                    while(tokens[i].end < endOfItems)
                    {
                        i++;
                        if(tokens[i].type != JSMN_ARRAY) continue;
                        int itemID = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                        int itemCount = std::stoi(Utils::Subchar(buffer, tokens[i+2].start, tokens[i+2].end));
                        decodedInventory.Add(itemID, itemCount);
                    }
                }
                else if(strcmp(parseProperty, "creatures") == 0)
                {
                    i++; //move into the creatures array token
                    int numOfCreatures = tokens[i].size;
                    for (int j = 0; j < numOfCreatures; j++)
                    {
                        int creatureId = std::stoi(Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end));
                        decodedCreatures.emplace_back(EntityManager::GetInstance()->GetEntityCopy<Creature>(creatureId));
                    }
                    i=i+numOfCreatures;
                }
                else i++;
            }
            decodedAreas.emplace_back(decodedId, decodedName, decodedDialogue, decodedInventory, decodedCreatures);
        }
    }
    return std::make_shared<std::vector<Area>>(decodedAreas);
}
