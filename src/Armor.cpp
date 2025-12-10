//
// Created by Sergio Prada on 17/06/24.
//
#include <vector>
#include "Armor.h"
#include "Utils.h"
#include "Log.h"

Armor::Armor(const unsigned int _id, const std::string& _name, const std::string& _description, const int _defense)
: Item(_id, _name, _description), defense(_defense)
{

}
Armor::Armor(const Armor &other)
: Item(other), defense(other.GetDefense())
{

}
Armor::Armor(Armor &&other) noexcept
: Item(std::move(other)), defense(other.GetDefense())
{

}
int Armor::GetDefense() const {
    return defense;
}

std::shared_ptr<void> Armor::DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager) {
    std::vector<Armor> Armors_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type != JSMN_OBJECT) continue;

        unsigned int decodedId{}; std::string decodedName; int decodedDefense{}; std::string decodedDescription;
        const char* objects[] = {"id", "name", "defense", "description"};
        for (const char* & object : objects)
        {
            std::string value = Utils::ValueDecoder(buffer, tokens, i, i+(tokens[i].size*2), object);
            if(strcmp(object, "id") == 0) decodedId = std::stoi(value);
            else if(strcmp(object, "name") == 0) decodedName = value;
            else if(strcmp(object, "defense") == 0) decodedDefense = std::stoi(value);
            else if(strcmp(object, "description") == 0) decodedDescription = value;
        }
        Armors_decoded.emplace_back(decodedId, decodedName, decodedDescription, decodedDefense);
        i+=(tokens[i].size*2);
    }
    return std::make_shared<std::vector<Armor>>(Armors_decoded);
}