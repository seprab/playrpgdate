//
// Created by Sergio Prada on 16/06/24.
//

#include "Entity.h"
#include "Log.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

Entity::Entity(const unsigned int _id):
id(_id), position(pdcpp::Point<int>(0, 0)), size(pdcpp::Point<int>(0, 0))
{
    name = nullptr;
    image_path = nullptr;
    description = nullptr;
    hp = 0;
    maxHP = 0;
    bitmap = nullptr;
}

void Entity::LoadBitmap()
{
    const char *outErr = nullptr;
    bitmap = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(image_path, &outErr);
    if (bitmap == nullptr)
    {
        Log::Error("Entity %d couldn't load bitmap %s: %s", id, image_path, outErr);
    }
}

void Entity::LoadBitmap(char *path)
{
    image_path = path;
    LoadBitmap();
}

void Entity::DrawBitmap()
{
    if (bitmap == nullptr)
    {
        Log::Error("Entity %d has no bitmap loaded", id);
        return;
    }
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawBitmap(bitmap, position.x, position.y, kBitmapUnflipped);
}

void Entity::DrawBitmap(int x, int y)
{
    SetPosition(pdcpp::Point<int>(x, y));
    DrawBitmap();
}

