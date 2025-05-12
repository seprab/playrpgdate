//
// Created by Sergio Prada on 26/12/24.
//

#include "Beam.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "EntityManager.h"
#include "Log.h"

Beam::Beam(pdcpp::Point<int> Position) :
Magic(Position), startPosition(position), endPosition(Position) {
    iLifetime = 2000;
    size = 1;
    explosionThreshold = 300;
}

void Beam::Draw() const {
    //drawLine(int x1, int y1, int x2, int y2, int width, LCDColor color);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawLine(startPosition.x, startPosition.y, endPosition.x, endPosition.y, (int)size, kColorWhite);
    if (exploding)
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawLine(startPosition.x, startPosition.y, endPosition.x, endPosition.y, -(int)size, kColorWhite);
}

void Beam::HandleInput()
{
    float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankAngle()* kPI /180.f;
    float startDistance = 30.f;
    float length = 150.f;
    position = EntityManager::GetInstance()->GetPlayer()->GetCenteredPosition();
    startPosition.x = position.x + (int)(cos(angle) * startDistance);
    startPosition.y = position.y + (int)(sin(angle) * startDistance);
    endPosition.x = position.x + (int)(cos(angle) * length);
    endPosition.y = position.y + (int)(sin(angle) * length);

    if (exploding)
    {
        size+=4;
        return;
    }
    exploding = iLifetime - elapsedTime < explosionThreshold;
}

void Beam::Damage(const std::shared_ptr<Area>& area)
{
    if (!exploding) return;
    int xi = startPosition.x;
    int xf = endPosition.x;
    int yi = startPosition.y;
    int yf = endPosition.y;

    for (auto& entity : area->GetCreatures())
    {
        pdcpp::Point<int> creaturePos = entity->GetPosition();

        // Calculate the perpendicular distance from the line to the point
        float numerator = abs((yf - yi) * (xi - creaturePos.x) - (xf - xi) * (yi - creaturePos.y));
        float denominator = sqrt(pow((yf - yi), 2) + pow((xf - xi), 2));
        float distance = numerator / denominator;
        if (distance < size)
        {
            entity->Damage(1);
        }
    }
}


