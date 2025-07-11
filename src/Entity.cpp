//
// Created by Sergio Prada on 16/06/24.
//

#include "Entity.h"

#include "Globals.h"
#include "Log.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

std::unordered_map<char*, LCDBitmap*> Entity::bitmapCache;

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
    if (!image_path) return;
    auto it = bitmapCache.find(image_path);
    if (it != bitmapCache.end())
    {
        bitmap = it->second;
        return;
    }

    const char *outErr = nullptr;
    bitmap = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(image_path, &outErr);

    if (bitmap == nullptr)
    {
        Log::Error("Entity %d couldn't load bitmap %s: %s", id, image_path, outErr);
    }
    else
    {
        bitmapCache[image_path] = bitmap;
    }
}

void Entity::LoadBitmap(char *path)
{
    image_path = path;
    LoadBitmap();
}

void Entity::DrawBitmap() const
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
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(position.x-5, position.y - 10, 25, 4, kColorWhite);
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(position.x-5, position.y - 10, static_cast<int>(25 * (static_cast<float>(hp) / maxHP)), 4, kColorWhite);
}

void Entity::SetPosition(pdcpp::Point<int> _position)
{
    position = _position;
    tiledPosition.x = _position.x / Globals::MAP_TILE_SIZE; // Assuming size.x is the tile width
    tiledPosition.y = _position.y / Globals::MAP_TILE_SIZE; // Assuming size.y is the tile height
}

void Entity::SetTiledPosition(pdcpp::Point<int> _tiledPosition)
{
    tiledPosition = _tiledPosition;
    position.x = _tiledPosition.x * Globals::MAP_TILE_SIZE; // Assuming size.x is the tile width
    position.y = _tiledPosition.y * Globals::MAP_TILE_SIZE; // Assuming size.y is the tile height
}

void Entity::Damage(float damage)
{
    //Log::Info("Entity %s took damage", name);
    hp = std::max(0.f, hp - damage);
}

void Entity::Heal(float heal) {

    hp = std::min(maxHP, hp + heal);
}

