#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include "Item.h"

class EntityManager;

class Weapon : public Item
{
private:
    int damage;

public:
    Weapon()=default;
    Weapon(unsigned int _id, char* _name, char* _description, int _damage);
    void * DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;
    int GetDamage();
};

#endif