//
// Created by Sergio Prada on 16/06/24.
//

#include "Entity.h"

#include "Globals.h"
#include "Log.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

std::unordered_map<std::string, LCDBitmap*> Entity::bitmapCache;

Entity::Entity(const unsigned int _id)
    : id(_id), position(pdcpp::Point<int>(0, 0)), size(pdcpp::Point<int>(0, 0))
{
    hp = 0;
    maxHP = 0;
    bitmap = nullptr;
}

void Entity::LoadBitmap()
{
    if (image_path.empty()) return;
    auto it = bitmapCache.find(image_path);
    if (it != bitmapCache.end())
    {
        bitmap = it->second;
        return;
    }

    const char *outErr = nullptr;
    bitmap = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(image_path.c_str(), &outErr);

    if (bitmap == nullptr)
    {
        Log::Error("Entity %d couldn't load bitmap %s: %s", id, image_path.c_str(), outErr);
    }
    else
    {
        bitmapCache[image_path] = bitmap;
    }
}

void Entity::LoadBitmap(const std::string& path)
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

bool Entity::CalculateFlashing()
{
    if (isFlashing)
    {
        flashTimer++;
        isBitmapVisible = !isBitmapVisible; // Toggle visibility
        if (flashTimer > Globals::MAX_ENTITY_FLASHING_TICKS)
        {
            isFlashing = false;
            flashTimer = 0;
            isBitmapVisible = true;
        }
        return isBitmapVisible;
    }
    return true; // If not flashing, always return true (visible)
}

void Entity::Draw()
{

    if (CalculateFlashing())
    {
        DrawBitmap();
    }
    DrawHealthBar();
}

void Entity::DrawHealthBar() const
{
    int barX = position.x + Globals::HEALTH_BAR_OFFSET_X;
    int barY = position.y + Globals::HEALTH_BAR_OFFSET_Y;
    int barWidth = static_cast<int>(Globals::HEALTH_BAR_WIDTH * (static_cast<float>(hp) / maxHP));

    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(barX, barY, Globals::HEALTH_BAR_WIDTH, Globals::HEALTH_BAR_HEIGHT, kColorWhite);
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillRect(barX, barY, barWidth, Globals::HEALTH_BAR_HEIGHT, kColorWhite);
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
    hp = std::max(0.f, hp - damage);
    isFlashing = true;
}

void Entity::Heal(float heal)
{
    hp = std::min(maxHP, hp + heal);
}

