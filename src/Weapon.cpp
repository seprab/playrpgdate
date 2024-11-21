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

Weapon::Weapon(const Weapon &other)
: Item(other), damage(other.damage)
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
        char* decodedName{nullptr};
        int decodedDamage{-1};
        char* decodedDescription{nullptr};

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
            else if(strcmp(parseProperty, "damage") == 0)
            {
                decodedDamage = atoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
                i+=2;
            }
            else if(strcmp(parseProperty, "description") == 0)
            {
                decodedDescription = Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end);
                i+=2;
            }
            else
            {
                i++;
            }
            if (decodedId!=0 && decodedDamage!=-1 && decodedDescription != nullptr && decodedName != nullptr)
            {
                weapons_decoded.emplace_back(decodedId, decodedName, decodedDescription, decodedDamage);
                i--;
                break;
            }
        }
    }
    return std::make_shared<std::vector<Weapon>>(weapons_decoded);
}