//
// Created by Sergio Prada on 9/01/25.
//

#include "OrbitingProjectiles.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "EntityManager.h"

OrbitingProjectiles::OrbitingProjectiles(pdcpp::Point<int> Position) :
        Magic(Position) {
    isAlive = true;
    iLifetime = 4000;
    size = 10;
    radius = 40;
    bornTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
}

void OrbitingProjectiles::Draw() const {
    for (int i=0; i< sizeof(angles) / sizeof(angles[0]); i++)
    {
        //fillEllipse(int x, int y, int width, int height, float startAngle, float endAngle, LCDColor color);
        int x = position.x + (int)(cos(angles[i]) * radius);
        int y = position.y + (int)(sin(angles[i]) * radius);
        pdcpp::GlobalPlaydateAPI::get()->graphics->fillEllipse(x, y, size, size, 0, 360, kColorWhite);
    }
}

void OrbitingProjectiles::HandleInput() {
    position = EntityManager::GetInstance()->GetPlayer()->GetCenteredPosition();
    float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankChange()* kPI /180.f;
    for (int i=0; i< sizeof(angles) / sizeof(angles[0]); i++)
    {
        angles[i] += angle;
    }
}
