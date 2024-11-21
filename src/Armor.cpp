//
// Created by Sergio Prada on 17/06/24.
//
#include <vector>
#include "Armor.h"
#include "Utils.h"

Armor::Armor(unsigned int _id, char *_name, char *_description, int _defense)
: Item(_id, _name, _description), defense(_defense)
{

}
Armor::Armor(const Armor &other)
: Item(other), defense(other.GetDefense())
{

}
Armor::Armor(Armor &&other) noexcept
: Item(other), defense(other.GetDefense())
{

}
int Armor::GetDefense() const {
    return defense;
}

std::shared_ptr<void> Armor::DecodeJson(char *buffer, jsmntok_t *tokens, int size) {
    std::vector<Armor> Armors_decoded;
    for (int i = 0; i < size; i++)
    {
        if (tokens[i].type != JSMN_OBJECT) continue;

        unsigned int decodedId{0};
        char* decodedName{nullptr};
        int decodedDefense{-1};
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
            else if(strcmp(parseProperty, "defense") == 0)
            {
                decodedDefense = atoi(Utils::Subchar(buffer, tokens[i+1].start, tokens[i+1].end));
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
            if (decodedId!=0 && decodedDefense!=-1 && decodedDescription != nullptr && decodedName != nullptr)
            {
                Armors_decoded.emplace_back(decodedId, decodedName, decodedDescription, decodedDefense);
                i--;
                break;
            }
        }
    }
    return std::make_shared<std::vector<Armor>>(Armors_decoded);
}

int Armor::GetDefense() {
    return defense;
}