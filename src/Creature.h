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
class Player;

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
    int movementScale{}; // Default movement scale, can be adjusted for different creature types

    Inventory* inventory{};
    Weapon* equippedWeapon{};
    Armor* equippedArmor{};

public:
    Creature()=default;
    Creature(const Creature& other);
    Creature(Creature&& other) noexcept;
    Creature(unsigned int _id, const std::string& _name, const std::string& _image, float _maxHp, int _strength, int _agility,
             int _constitution, float _evasion, unsigned int _xp, int weapon, int armor);


    [[nodiscard]] int GetStrength() const;
    void SetStrength(int _strength);
    [[nodiscard]] int GetAgility() const;
    void SetAgility(int _agility);
    [[nodiscard]] int GetConstitution() const;
    void SetConstitution(int _constitution);
    [[nodiscard]] float GetEvasion() const;
    void SetEvasion(float _evasion);
    [[nodiscard]] unsigned int GetXP() const;
    void SetMovementScale(int value){movementScale = value;}


    Inventory* GetInventory();
    Weapon* GetEquippedWeapon();
    Armor* GetEquippedArmor();
    [[nodiscard]] int GetMovementSpeed() const {return movementScale; }

    void EquipWeapon(Weapon* weapon);
    void EquipArmor(Armor* armor);

    int Attack(Creature* target);
    int TraverseDoor(Door* door);
    void Tick();
};

#endif