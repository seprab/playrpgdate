//
// Created by Sergio Prada on 9/01/25.
//

#include "OrbitingProjectiles.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "EntityManager.h"
#include "Monster.h"

OrbitingProjectiles::OrbitingProjectiles(const pdcpp::Point<int> Position, std::weak_ptr<Player> _player) :
        Magic(Position, std::move(_player)) {
    iLifetime = 4000;
    size = 10;
    radius = 40;
}

void OrbitingProjectiles::Draw() const
{
    for (const auto& pos : projectilePositions)
    {
        //fillEllipse(int x, int y, int width, int height, float startAngle, float endAngle, LCDColor color);
        pdcpp::GlobalPlaydateAPI::get()->graphics->fillEllipse(pos.x, pos.y, static_cast<int>(size), static_cast<int>(size), 0, 360, kColorWhite);
    }
}

void OrbitingProjectiles::HandleInput() {
    std::shared_ptr<Player> p = player.lock();
    if (!p) return; // player was destroyed
    position = p->GetCenteredPosition();
    const float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankChange()* kPI /180.f;

    for (int i=0; i< sizeof(angles) / sizeof(angles[0]); i++)
    {
        angles[i] += angle;
        const int x = position.x + static_cast<int>(cos(angles[i]) * radius);
        const int y = position.y + static_cast<int>(sin(angles[i]) * radius);
        projectilePositions[i].x = x;
        projectilePositions[i].y = y;
    }
}

void OrbitingProjectiles::Damage(const std::shared_ptr<Area>& area)
{
    std::shared_ptr<Player> p = player.lock();
    if (!p) return; //Player has been destroyed
    position = p->GetCenteredPosition();
    for (const auto& entity : area->GetCreatures())
    {
        const pdcpp::Point<int> creaturePos = entity->GetPosition();
        // Calculate the distance between the projectile and the creature as a first filter,
        auto distance = static_cast<float>(sqrt(pow(position.x - creaturePos.x, 2) + pow(position.y - creaturePos.y, 2)));
        if (distance < static_cast<float>(radius + (size/2)) && distance > static_cast<float>(radius - (size/2)))
        {
            // Calculate the angle between the projectile and the creature.
            const auto angle = static_cast<float>(atan2(creaturePos.y - position.y, creaturePos.x - position.x));
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
