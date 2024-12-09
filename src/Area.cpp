#include <utility>
#include "Area.h"
#include "Door.h"
#include "Entity.h"
#include "Inventory.h"
#include "Dialogue.h"
#include "Log.h"
#include "Utils.h"

Area::Area(unsigned int _id, char* _name, std::shared_ptr<Dialogue> _dialogue, std::vector<std::shared_ptr<Creature>> _creatures)
: Entity(_id), dialogue(std::move(_dialogue))
{
    SetName(_name);
    for (const std::shared_ptr<Creature>& creature : _creatures)
    {
        _creatures.push_back(creature);
    }
    Log::Info("Area created with id: %d, name: %s", _id, _name);
}
Area::Area(const Area &other)
        : Entity(other.GetId()), dialogue(other.dialogue), creatures(other.creatures)
{
    SetName(other.GetName());
}
Area::Area(Area &&other) noexcept
        : Entity(other.GetId()), dialogue(std::move(other.dialogue)), creatures(other.creatures)
{
    SetName(other.GetName());
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
            std::vector<std::shared_ptr<Creature>> decodedCreatures;

            const int endOfObject = tokens[i].end; //get the end of the Area object
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
                        auto originalInstance = EntityManager::GetInstance()->GetEntity(decodedDoor);
                        if (originalInstance == nullptr)
                        {
                            Log::Error("Door with ID %d not found", decodedDoor);
                            continue;
                        }
                        decodedDoors.push_back(std::static_pointer_cast<Door>(originalInstance));
                    }
                    i=i+numOfDoors;
                }
                else if(strcmp(parseProperty, "creatures") == 0)
                {
                    i++; //move into the creatures array token
                    int numOfCreatures = tokens[i].size;
                    i++; //either move to the first element of the array or move to the next property
                    for (int j = 0; j < numOfCreatures; j++)
                    {
                        int creatureId = std::stoi(Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end));
                        auto originalInstance = EntityManager::GetInstance()->GetEntity(creatureId);
                        if (originalInstance == nullptr)
                        {
                            Log::Error("Creature with ID %d not found", creatureId);
                            continue;
                        }
                        decodedCreatures.push_back(std::static_pointer_cast<Creature>(originalInstance));
                    }
                    i=i+numOfCreatures;
                }
                else i++;
            }
            decodedAreas.emplace_back(decodedId, decodedName, decodedDialogue, decodedCreatures);
            i--; //move back to the end of the Area object
        }
    }
    return std::make_shared<std::vector<Area>>(decodedAreas);
}
