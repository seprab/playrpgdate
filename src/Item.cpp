#include <utility>
#include "Item.h"

Item::Item(std::string _id, char* itemName, char* itemDescription) :
Entity(std::move(_id)), name(itemName), description(itemDescription)
{

}
const char* Item::GetName() const
{
    return name;
}
const char* Item::GetDescription() const
{
    return description;
}
