//
// Created by Sergio Prada on 9/01/25.
//

#include "OrbitingProjectiles.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "EntityManager.h"
#include "Log.h"
#include "Monster.h"

OrbitingProjectiles::OrbitingProjectiles(pdcpp::Point<int> Position) :
        Magic(Position) {
    iLifetime = 4000;
    size = 10;
    radius = 40;
}

void OrbitingProjectiles::Draw() const
{
    for (int i=0; i< sizeof(projectilePositions) / sizeof(projectilePositions[0]); i++)
    {
        //fillEllipse(int x, int y, int width, int height, float startAngle, float endAngle, LCDColor color);
        pdcpp::GlobalPlaydateAPI::get()->graphics->fillEllipse(projectilePositions[i].x, projectilePositions[i].y, size, size, 0, 360, kColorWhite);
    }
}

void OrbitingProjectiles::HandleInput() {
    position = EntityManager::GetInstance()->GetPlayer()->GetCenteredPosition();
    float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankChange()* kPI /180.f;
    for (int i=0; i< sizeof(angles) / sizeof(angles[0]); i++)
    {
        angles[i] += angle;
        int x = position.x + (int)(cos(angles[i]) * radius);
        int y = position.y + (int)(sin(angles[i]) * radius);
        projectilePositions[i].x = x;
        projectilePositions[i].y = y;
    }
}

void OrbitingProjectiles::Damage(const std::shared_ptr<Area>& area)
{
    position = EntityManager::GetInstance()->GetPlayer()->GetCenteredPosition();
    for (auto& entity : area->GetCreatures())
    {
        pdcpp::Point<int> creaturePos = entity->GetPosition();
        // Calculate the distance between the projectile and the creature as a first filter,
        float distance = sqrt(pow(position.x - creaturePos.x, 2) + pow(position.y - creaturePos.y, 2));
        if (distance < (radius + (size/2)) && distance > (radius - (size/2)))
        {
            // Calculate the angle between the projectile and the creature.
            float angle = atan2(creaturePos.y - position.y, creaturePos.x - position.x);
            //Log::Info("Angle creature: %f, angles projectile: %f", angle, angles[0]);
            //angles[0] comes in the range of 0 to 2*PI
            //angle comes in the range of -PI to PI
            for (int i=0; i< sizeof(angles) / sizeof(angles[0]); i++)
            {
                // Normalize the angle to the range of 0 to 2*PI
                float normalizedAngle = fmod(angle + 2 * kPI, 2 * kPI);
                if (abs(normalizedAngle - angles[i]) < 0.2f)
                {
                    // Damage the creature
                    entity->Damage(0.1f);
                    break;
                }
            }
        }
    }
}
