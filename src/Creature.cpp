//
// Created by Sergio Prada on 16/06/24.
//
#include <vector>
#include "Creature.h"
#include "Utils.h"
#include "Log.h"

Creature::Creature(unsigned int _id, const std::string& _name, const std::string& image, float _maxHp, int _strength, int _agility, int _constitution,
                   float _evasion, unsigned int _xp, int _weapon, int _armor)
                   : Entity(_id), strength(_strength), agility(_agility),
                   constitution(_constitution), evasion(_evasion), xp(_xp), weapon(_weapon), armor(_armor)
                   {
    SetImagePath(image);
    SetName(_name);
    SetHP(_maxHp);
    SetMaxHP(_maxHp);
    SetMovementScale(1); // Default movement scale for creatures, can be adjusted later
}

Creature::Creature(const Creature &other)
: Entity(other), strength(other.GetStrength()), agility(other.GetAgility())
{
    SetHP(other.GetHP());
    SetMaxHP(other.GetMaxHP());
    SetMovementScale(3); // Default movement scale for creatures, can be adjusted later
}
Creature::Creature(Creature &&other) noexcept
: Entity(std::move(other)), strength(other.GetStrength()), agility(other.GetAgility())
{
    SetHP(other.GetHP());
    SetMaxHP(other.GetMaxHP());
    SetMovementScale(1); // Default movement scale for creatures, can be adjusted later
}

int Creature::GetStrength() const {
    return strength;
}

void Creature::SetStrength(int _strength) {
    strength = _strength;
}

int Creature::GetAgility() const {
    return agility;
}

void Creature::SetAgility(int _agility) {
    agility = _agility;
}

int Creature::GetConstitution() const {
    return constitution;
}

void Creature::SetConstitution(int _constitution) {
    constitution = _constitution;
}

float Creature::GetEvasion() const {
    return evasion;
}

void Creature::SetEvasion(float _evasion) {
    evasion = _evasion;
}

unsigned int Creature::GetXP() const {
    return xp;
}

Inventory *Creature::GetInventory() {
    return inventory;
}

Weapon *Creature::GetEquippedWeapon() {
    return equippedWeapon;
}

Armor *Creature::GetEquippedArmor() {
    return equippedArmor;
}

void Creature::EquipWeapon(Weapon *_weapon) {
    equippedWeapon = _weapon;
}

void Creature::EquipArmor(Armor *_armor) {
    equippedArmor = _armor;
}

int Creature::Attack(Creature *target) {
    target->Damage(static_cast<float>(GetStrength()));
}

int Creature::TraverseDoor(Door *door) {
    return 0;
}

void Creature::Tick()
{
    Draw();
}


