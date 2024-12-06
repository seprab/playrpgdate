//
// Created by Sergio Prada on 16/06/24.
//

#include "Entity.h"
#include "Log.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

Entity::Entity(const unsigned int _id):
id(_id)
{
    name = nullptr;
    image_path = nullptr;
    description = nullptr;
    position = std::make_pair(0, 0);
    size = std::make_pair(0, 0);
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
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawBitmap(bitmap, position.first, position.second, kBitmapUnflipped);
}

void Entity::DrawBitmap(int x, int y)
{
    SetPosition(std::make_pair(x, y));
    DrawBitmap();
}

