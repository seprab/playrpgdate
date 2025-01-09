//
// Created by Sergio Prada on 26/12/24.
//

#include "Beam.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "EntityManager.h"

Beam::Beam(pdcpp::Point<int> Position) :
Magic(Position), startPosition(position), endPosition(Position) {
    isAlive = true;
    iLifetime = 2000;
    size = 1;
    explosionThreshold = 300;
    bornTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
}

void Beam::Draw() const {
    //drawLine(int x1, int y1, int x2, int y2, int width, LCDColor color);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawLine(startPosition.x, startPosition.y, endPosition.x, endPosition.y, (int)size, kColorWhite);
    if (exploding)
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawLine(startPosition.x, startPosition.y, endPosition.x, endPosition.y, -(int)size, kColorWhite);
}

void Beam::HandleInput() {
    if (exploding)
    {
        size+=4;
        return;
    }

    exploding = iLifetime - elapsedTime < explosionThreshold;
    float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankAngle()* kPI /180.f;

    float startDistance = 30.f;
    float length = 150.f;
    position = EntityManager::GetInstance()->GetPlayer()->GetCenteredPosition();
    startPosition.x = position.x + (int)(cos(angle) * startDistance);
    startPosition.y = position.y + (int)(sin(angle) * startDistance);
    endPosition.x = position.x + (int)(cos(angle) * length);
    endPosition.y = position.y + (int)(sin(angle) * length);
}


