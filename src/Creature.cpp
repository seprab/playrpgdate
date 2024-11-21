//
// Created by Sergio Prada on 16/06/24.
//
#include <vector>
#include "Creature.h"
#include "Utils.h"

Creature::Creature(unsigned int _id, char *_name, float _maxHp, int _strength, int _agility, int _constitution,
                   float _evasion, unsigned int _xp, int _weapon, int _armor)
                   : Entity(_id), name(_name), maxHP(_maxHp), strength(_strength), agility(_agility),
                   constitution(_constitution), evasion(_evasion), xp(_xp), weapon(_weapon), armor(_armor)
                   {
    hp=maxHP;

}

Creature::Creature(const Creature &other)
: Entity(other), name(other.name), maxHP(other.GetMaxHP()), strength(other.GetStrength()), agility(other.GetAgility())
{
}
Creature::Creature(Creature &&other) noexcept
: Entity(other), name(other.name), maxHP(other.GetMaxHP()), strength(other.GetStrength()), agility(other.GetAgility())
{
}

std::shared_ptr<void> Creature::DecodeJson(char *buffer, jsmntok_t *tokens, int size) {
    std::vector<Creature> creatures_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type != JSMN_OBJECT) continue;

        unsigned int decodedId{0};
        char* decodedName{nullptr};
        float decodedMaxHp{-1};
        int decodedStrength{-1};
        int decodedAgility{-1};
        int decodedConstitution{-1};
        float decodedEvasion{-1};
        unsigned int decodedXp{0};
        int decodedWeapon{-1};
        int decodedArmor{-1};

        while(i<tokens[size].end)
        {
            if (tokens[i].type != JSMN_STRING)
            {
                i++;
                continue;
            }
            char* parseProperty = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);

            if(strcmp(parseProperty, "id") == 0)
            {
                decodedId = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "name") == 0)
            {
                decodedName = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
                i+=2;
            }
            else if(strcmp(parseProperty, "hp") == 0)
            {
                decodedMaxHp = atof(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "str") == 0)
            {
                decodedStrength = atoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "agi") == 0)
            {
                decodedAgility = atoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "con") == 0)
            {
                decodedConstitution = atoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "evasion") == 0)
            {
                decodedEvasion = atof(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "xp") == 0)
            {
                decodedXp = std::stoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "weapon") == 0)
            {
                decodedWeapon = atoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "armor") == 0)
            {
                decodedArmor = atoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else
            {
                i++;
            }
            if (decodedId!=0 && decodedName != nullptr && decodedMaxHp!=-1 && decodedStrength!=-1 &&
            decodedAgility!=-1 && decodedConstitution!=-1 && decodedEvasion!=-1 && decodedXp!=0 &&
            decodedWeapon!=-1 && decodedArmor!=-1)
            {
                //TODO: I need to set the inventory, armor and weapon.
                creatures_decoded.emplace_back(decodedId, decodedName, decodedMaxHp, decodedStrength, decodedAgility, decodedConstitution, decodedEvasion, decodedXp, decodedWeapon, decodedArmor);
                i--;
                break;
            }
        }
    }
    return std::make_shared<std::vector<Creature>>(creatures_decoded);
}

char *Creature::GetName() {
    return name;
}

float Creature::GetHP() const {
    return hp;
}

void Creature::SetHP(float _hp) {
    hp = _hp;
}

float Creature::GetMaxHP() const {
    return maxHP;
}

void Creature::SetMaxHP(float _maxHP) {
    maxHP = _maxHP;
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


