//
// Created by Sergio Prada on 16/06/24.
//
#include <vector>
#include "Creature.h"
#include "Utils.h"
#include "Log.h"

Creature::Creature(unsigned int _id, char *_name, char* image, float _maxHp, int _strength, int _agility, int _constitution,
                   float _evasion, unsigned int _xp, int _weapon, int _armor)
                   : Entity(_id), strength(_strength), agility(_agility),
                   constitution(_constitution), evasion(_evasion), xp(_xp), weapon(_weapon), armor(_armor)
                   {
    SetImagePath(image);
    SetName(_name);
    SetHP(_maxHp);
    SetMaxHP(_maxHp);

}

Creature::Creature(const Creature &other)
: Entity(other), strength(other.GetStrength()), agility(other.GetAgility())
{
    SetHP(other.GetHP());
    SetMaxHP(other.GetMaxHP());
}
Creature::Creature(Creature &&other) noexcept
: Entity(other), strength(other.GetStrength()), agility(other.GetAgility())
{
    SetHP(other.GetHP());
    SetMaxHP(other.GetMaxHP());
}

std::shared_ptr<void> Creature::DecodeJson(char *buffer, jsmntok_t *tokens, int size)
{
    std::vector<Creature> creatures_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type != JSMN_OBJECT) continue;

        unsigned int decodedId; char* decodedName; char* decodedPath; float decodedMaxHp; int decodedStrength;
        int decodedAgility; int decodedConstitution; float decodedEvasion; unsigned int decodedXp;
        int decodedWeapon; int decodedArmor;

        const char* objects[] = {"id", "name", "image", "hp", "str", "agi", "con", "evasion", "xp", "weapon", "armor"};
        for (const char* & object : objects)
        {
            // doing (tokens[i].size*2) because the object size returns the number of elements inside.
            // for example:
            // { "id": 1, "name": "Sergio" } its size = 2. But the tokens are 4.
            char* value = Utils::ValueDecoder(buffer, tokens, i, i+(tokens[i].size*2), object);

            if(strcmp(object, "id") == 0) decodedId = atoi(value);
            else if(strcmp(object, "name") == 0) decodedName = value;
            else if(strcmp(object, "image") == 0) decodedPath = value;
            else if(strcmp(object, "hp") == 0) decodedMaxHp = atof(value);
            else if(strcmp(object, "str") == 0) decodedStrength = atoi(value);
            else if(strcmp(object, "agi") == 0) decodedAgility = atoi(value);
            else if(strcmp(object, "con") == 0) decodedConstitution = atoi(value);
            else if(strcmp(object, "evasion") == 0) decodedEvasion = atof(value);
            else if(strcmp(object, "xp") == 0) decodedXp = std::stoi(value);
            else if(strcmp(object, "weapon") == 0) decodedWeapon = atoi(value);
            else if(strcmp(object, "armor") == 0) decodedArmor = atoi(value);
            else Log::Error("Unknown property %s", object);
        }
        creatures_decoded.emplace_back(decodedId, decodedName, decodedPath, decodedMaxHp, decodedStrength,
                                       decodedAgility, decodedConstitution, decodedEvasion, decodedXp,
                                       decodedWeapon, decodedArmor);
        Log::Info("Creature ID: %d, name %s", decodedId, decodedName);

        i+=(tokens[i].size*2);
    }
    return std::make_shared<std::vector<Creature>>(creatures_decoded);
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
    return 0;
}

int Creature::TraverseDoor(Door *door) {
    return 0;
}

void Creature::Tick()
{
    Draw();
}


