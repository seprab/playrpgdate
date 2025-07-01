//
// Created by Sergio Prada on 26/12/24.
//

#include "Projectile.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"
#include "Monster.h"

Projectile::Projectile(pdcpp::Point<int> Position):
Magic(Position)
{
    iLifetime = 2000;
    speed = 8;
    size = 6;
    explosionThreshold = 800;
    launchAngle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankAngle() * kPI /180.f;
}

void Projectile::Draw() const
{
    //void playdate->graphics->drawEllipse(int x, int y, int width, int height, int lineWidth, float startAngle, float endAngle, LCDColor color);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawEllipse(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(size), static_cast<int>(size), 1, 0, 0, kColorWhite);
    int fillSize = static_cast<int>(size) - 3;
    if (exploding && fillSize > 0)
    {
        //fillEllipse(int x, int y, int width, int height, float startAngle, float endAngle, LCDColor color);
        pdcpp::GlobalPlaydateAPI::get()->graphics->fillEllipse(position.x, position.y, fillSize, fillSize, 0, 0, kColorWhite);
    }
}

void Projectile::HandleInput()
{
    if (exploding)
    {
        size+=sizeIncrement;
        // Since the ellipse is drawn from the top left corner, we need to adjust the position
        int positionAdjustment = static_cast<int>(sizeIncrement / 2);
        position.x -= positionAdjustment;
        position.y -= positionAdjustment;
        return;
    }
    exploding = elapsedTime > explosionThreshold;
    position.x += static_cast<int>(cos(launchAngle) * speed);
    position.y += static_cast<int>(sin(launchAngle) * speed);
}

void Projectile::Damage(const std::shared_ptr<Area>& area)
{
    auto projectileCenterdPos = GetCenteredPosition();
    for (const auto& entity : area->GetCreatures())
    {
        float distance = projectileCenterdPos.distance(entity->GetPosition());
        if (distance < static_cast<float>(size)/2.f)
        {
            entity->Damage(0.5f);
        }
    }
}

pdcpp::Point<int> Projectile::GetCenteredPosition() const
{
    return {static_cast<int>(position.x + size / 2), static_cast<int>(position.y + size / 2)};
}
