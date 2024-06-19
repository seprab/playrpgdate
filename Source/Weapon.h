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
    Weapon(std::string _id, std::string _name, std::string _description, int _damage);

    int GetDamage();

};

#endif