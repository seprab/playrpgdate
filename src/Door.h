#ifndef DOOR_H
#define DOOR_H

#include <string>
#include <utility>

#include "Entity.h"

class Item;
class EntityManager;

class Door : public Entity
{
private:
    std::string description;
    int locked;
    Item* key;
    std::pair<std::string, std::string> areas;

public:
    Door(unsigned int _id, std::string _description, int _locked, Item* _key = nullptr, std::pair<std::string, std::string> _areas = std::make_pair("", ""));

    std::string GetDescription();
    int GetLocked() const;
    void SetLocked(int _locked);

    Item* GetKey();

    std::pair<std::string, std::string> GetArea();
    void SetArea(std::pair<std::string, std::string> _areas);
    void * DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;
};

#endif