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
    bool locked;
    int key;
    std::pair<int, int> areas;

public:
    Door()=default;
    Door(int _id, bool _locked, int _keyId, int _areaA, int _areaB);

    int GetLocked() const;
    void SetLocked(int _locked);
    int GetKey();
    std::pair<int, int> GetAreas();
    void * DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;
};

#endif