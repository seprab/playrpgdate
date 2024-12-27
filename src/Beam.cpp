//
// Created by Sergio Prada on 26/12/24.
//

#include "Beam.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "EntityManager.h"

Beam::Beam(pdcpp::Point<int> Position) :
Magic(Position), endPosition(Position) {
    isAlive = true;
    iLifetime = 2000;
    size = 1;
    explosionThreshold = 300;
    bornTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
}

void Beam::Draw() const {
    //drawLine(int x1, int y1, int x2, int y2, int width, LCDColor color);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawLine(position.x, position.y, endPosition.x, endPosition.y, (int)size, kColorWhite);
    if (exploding)
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawLine(position.x, position.y, endPosition.x, endPosition.y, -(int)size, kColorWhite);
}

void Beam::HandleInput() {
    if (exploding)
    {
        size+=4;
        return;
    }

    exploding = iLifetime - elapsedTime < explosionThreshold;
    float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankAngle();
    angle = angle * kPI /180.f;

    float distance = 150.f;
    UpdatePosition();
    endPosition.x = position.x + (int)(cos(angle) * distance);
    endPosition.y = position.y + (int)(sin(angle) * distance);
}

void Beam::UpdatePosition()
{
    auto pos = EntityManager::GetInstance()->GetPlayer()->GetPosition();
    position = pos;
}


