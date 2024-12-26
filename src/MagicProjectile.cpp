//
// Created by Sergio Prada on 26/12/24.
//

#include "MagicProjectile.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"

MagicProjectile::MagicProjectile(pdcpp::Point<int> Position, float Speed, int Size) :
position(Position), speed(Speed), size(Size)
{
    isAlive = true;
    iLifetime = 5000;
    bornTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
}

void MagicProjectile::Update()
{
    if(!isAlive) return;

    unsigned int currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    if (currentTime - bornTime > iLifetime)
    {
        Kill();
    }

    HandleInput();
    Draw();
}

void MagicProjectile::Draw()
{
    //void playdate->graphics->drawEllipse(int x, int y, int width, int height, int lineWidth, float startAngle, float endAngle, LCDColor color);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawEllipse(position.x, position.y, size, size, 1, 0, 0, kColorWhite);
}

void MagicProjectile::Kill()
{
    isAlive = false;
}

void MagicProjectile::HandleInput()
{
    float change = pdcpp::GlobalPlaydateAPI::get()->system->getCrankChange();
    Log::Info("Crank change: %f", change);
}
