//
// Created by Sergio Prada on 17/06/24.
//
#include <vector>
#include "Weapon.h"
#include "Utils.h"

Weapon::Weapon(unsigned int _id, char* _name, char* _description, int _damage)
: Item(_id, _name, _description), damage(_damage)
{

}
Weapon::Weapon(Weapon &&other) noexcept
: Item(other), damage(other.damage)
{

}
int Weapon::GetDamage() const {
    return damage;
}

std::shared_ptr<void> Weapon::DecodeJson(char *buffer, jsmntok_t *tokens, int size)
{
    std::vector<Weapon> weapons_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type != JSMN_OBJECT) continue;

        unsigned int decodedId{0};
        char* decodedName{};
        int decodedDamage{};
        char* decodedDescription{};

        const char* objects[] = {"id", "name", "damage", "description"};
        for (const char* & object : objects)
        {
            char* value = Utils::ValueDecoder(buffer, tokens, i, i+(tokens[i].size*2), object);
            if(strcmp(object, "id") == 0) decodedId = atoi(value);
            else if(strcmp(object, "name") == 0) decodedName = value;
            else if(strcmp(object, "description") == 0) decodedDescription = value;
            else if(strcmp(object, "damage") == 0) decodedDamage = atoi(value);
        }
        weapons_decoded.emplace_back(decodedId, decodedName, decodedDescription, decodedDamage);
        i+=(tokens[i].size*2);
    }
    return std::make_shared<std::vector<Weapon>>(weapons_decoded);
}