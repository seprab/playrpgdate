//
// Created by Sergio Prada on 16/12/24.
//

#include "AutoProjectile.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"
#include "Monster.h"
#include <cmath>

AutoProjectile::AutoProjectile(pdcpp::Point<int> Position, std::weak_ptr<Player> _player, std::weak_ptr<Monster> _target):
Magic(Position, std::move(_player)), target(std::move(_target))
{
    iLifetime = 3000; // 3 seconds max lifetime
    speed = 6.0f;
    size = 4;
}

void AutoProjectile::Draw() const
{
    // Draw a simple filled circle for the auto projectile
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillEllipse(
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(size),
        static_cast<int>(size),
        0, 0, kColorWhite
    );
}

void AutoProjectile::HandleInput()
{
    // Check if target is still alive
    auto targetPtr = target.lock();
    if (!targetPtr || !targetPtr->IsAlive())
    {
        Terminate();
        return;
    }

    // Calculate direction to target
    pdcpp::Point<int> targetPos = targetPtr->GetCenteredPosition();
    int dx = targetPos.x - position.x;
    int dy = targetPos.y - position.y;

    // Calculate distance
    float distance = std::sqrt(static_cast<float>(dx * dx + dy * dy));

    if (distance < 3.0f)
    {
        // Close enough to hit
        Terminate();
        return;
    }

    // Normalize and move towards target
    if (distance > 0)
    {
        position.x += static_cast<int>((dx / distance) * speed);
        position.y += static_cast<int>((dy / distance) * speed);
    }
}

void AutoProjectile::Damage(const std::shared_ptr<Area>& area)
{
    auto targetPtr = target.lock();
    if (!targetPtr || !targetPtr->IsAlive())
    {
        return;
    }

    auto projectileCenteredPos = GetCenteredPosition();
    float distance = projectileCenteredPos.distance(targetPtr->GetCenteredPosition());

    if (distance < static_cast<float>(size))
    {
        targetPtr->Damage(0.3f); // Small damage for passive ability
        Terminate();
    }
}

pdcpp::Point<int> AutoProjectile::GetCenteredPosition() const
{
    return {static_cast<int>(position.x + size / 2), static_cast<int>(position.y + size / 2)};
}
