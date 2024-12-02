//
// Created by Sergio Prada on 16/06/24.
//

#include "Entity.h"

Entity::Entity(const unsigned int _id):
id(_id)
{

}

Entity::~Entity()
= default;

unsigned int Entity::GetId() const {
    return id;
}
