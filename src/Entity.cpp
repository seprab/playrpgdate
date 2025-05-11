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
    else
    {
        pdcpp::GlobalPlaydateAPI::get()->graphics->getBitmapData(bitmap, &size.x, &size.y, nullptr, nullptr, nullptr);
        Log::Info("Entity %d loaded bitmap %s", id, image_path);
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

void Entity::Draw()
{
    DrawBitmap();
    DrawHealthBar();
}

void Entity::DrawHealthBar() const
{
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(position.x-5, position.y - 10, 25, 2, kColorWhite);
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(position.x-5, position.y - 10, 25 * (hp / maxHP), 2, kColorWhite);
}

