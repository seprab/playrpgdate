//
// Created by Sergio Prada on 26/12/24.
//

#include "MagicProjectile.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"

MagicProjectile::MagicProjectile(pdcpp::Point<int> Position):
position(Position)
{
    isAlive = true;
    currentSize = travelSize;
    bornTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
}

void MagicProjectile::Update()
{
    if(!isAlive) return;

    unsigned int currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    unsigned int elapsedTime = currentTime - bornTime;
    if (!exploding)
    {
        exploding = iLifetime - elapsedTime < explosionThreshold;
        HandleInput();
    }
    else
    {
        if (elapsedTime > iLifetime)
        {
            Kill();
        }
        currentSize+=5;
    }
    Draw();
}

void MagicProjectile::Draw() const
{
    //void playdate->graphics->drawEllipse(int x, int y, int width, int height, int lineWidth, float startAngle, float endAngle, LCDColor color);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawEllipse(position.x, position.y, currentSize, currentSize, 1, 0, 0, kColorWhite);
    if (exploding)
    {
        //fillEllipse(int x, int y, int width, int height, float startAngle, float endAngle, LCDColor color);
        pdcpp::GlobalPlaydateAPI::get()->graphics->fillEllipse(position.x, position.y, currentSize-3, currentSize-3, 0, 0, kColorWhite);
    }
}

void MagicProjectile::Kill()
{
    isAlive = false;
}

void MagicProjectile::HandleInput()
{
    float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankAngle();

    angle = angle * kPI /180.f;

    position.x += cos(angle) * speed;
    position.y += sin(angle) * speed;
}
