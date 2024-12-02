#ifndef ARMOR_H
#define ARMOR_H

#include <string>
#include "Item.h"

class EntityManager;

class Armor : public Item
{
private:
    int defense=0;

public:
    Armor()=default;
    Armor(unsigned int _id, char* _name, char* _description, int _defense);
    Armor(const Armor& other);
    Armor(Armor&& other) noexcept;

    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;
    [[nodiscard]] int GetDefense() const;
};

#endif