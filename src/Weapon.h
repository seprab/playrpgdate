#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include "Item.h"

class EntityManager;

class Weapon : public Item
{
private:
    int damage=0;

public:
    Weapon()=default;
    Weapon(const Weapon& other) = default;
    Weapon(Weapon&& other) noexcept;
    Weapon(unsigned int _id, char* _name, char* _description, int _damage);
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;
    [[nodiscard]] int GetDamage() const;
};

#endif