//
// Created by Sergio Prada on 26/12/24.
//

#include "Projectile.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"

Projectile::Projectile(pdcpp::Point<int> Position):
Magic(Position)
{
    iLifetime = 2000;
    speed = 8;
    size = 10;
    explosionThreshold = 500;
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
        size+=5;
        return;
    }
    exploding = iLifetime - elapsedTime < explosionThreshold;
    float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankAngle();

    angle = angle * kPI /180.f;

    position.x += (int)(cos(angle) * speed);
    position.y += (int)(sin(angle) * speed);
}
