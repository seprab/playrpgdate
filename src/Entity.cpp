//
// Created by Sergio Prada on 16/06/24.
//

#include "Entity.h"

#include <utility>

Entity::Entity(std::string _id):
id(std::move(_id))
{

}

Entity::~Entity()
= default;

std::string Entity::GetID() const {
    return id;
}
