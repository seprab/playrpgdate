#ifndef CREATURE_H
#define CREATURE_H

#include "Entity.h"
#include "Inventory.h"
#include "Area.h"

class Area;
class EntityManager;
class Weapon;
class Armor;
class Door;

class Creature : public Entity
{
private:
    int strength{};
    int agility{};
    int constitution{};
    float evasion{};
    unsigned int xp{};
    int weapon{};
    int armor{};

    Inventory* inventory{};
    Weapon* equippedWeapon{};
    Armor* equippedArmor{};

public:
    Creature()=default;
    Creature(const Creature& other);
    Creature(Creature&& other) noexcept;
    Creature(unsigned int _id, char* _name, float _maxHp, int _strength, int _agility,
             int _constitution, float _evasion, unsigned int _xp, int weapon, int armor);
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;


    [[nodiscard]] int GetStrength() const;
    void SetStrength(int _strength);
    [[nodiscard]] int GetAgility() const;
    void SetAgility(int _agility);
    [[nodiscard]] int GetConstitution() const;
    void SetConstitution(int _constitution);
    [[nodiscard]] float GetEvasion() const;
    void SetEvasion(float _evasion);
    [[nodiscard]] unsigned int GetXP() const;


    Inventory* GetInventory();
    Weapon* GetEquippedWeapon();
    Armor* GetEquippedArmor();

    void EquipWeapon(Weapon* weapon);
    void EquipArmor(Armor* armor);

    int Attack(Creature* target);
    int TraverseDoor(Door* door);
};

#endif