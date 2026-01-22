//
// Created for enemy ranged attacks
//

#include "EnemyProjectile.h"
#include "Area.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"
#include "Player.h"
#include <cmath>

EnemyProjectile::EnemyProjectile(pdcpp::Point<int> Position, std::weak_ptr<Player> _player,
                                 float angle, float _speed, unsigned int _size, float _damage)
    : Magic(Position, std::move(_player))
{
    iLifetime = 3000; // 3 seconds max lifetime
    speed = _speed;
    size = _size;
    launchAngle = angle;
    damagePerHit = _damage;
}

void EnemyProjectile::Draw() const
{
    // Draw enemy projectile with a different style - filled circle with outline
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawEllipse(
        static_cast<int>(position.x), 
        static_cast<int>(position.y), 
        static_cast<int>(size), 
        static_cast<int>(size), 
        1, 0, 0, kColorWhite);
    // Fill with a slightly smaller circle to create outline effect
    int fillSize = static_cast<int>(size) - 2;
    if (fillSize > 0)
    {
        pdcpp::GlobalPlaydateAPI::get()->graphics->fillEllipse(
            position.x + 1, 
            position.y + 1, 
            fillSize, 
            fillSize, 
            0, 0, kColorBlack);
    }
}

void EnemyProjectile::HandleInput()
{
    // Move projectile in the direction it was launched
    position.x += static_cast<int>(cos(launchAngle) * speed);
    position.y += static_cast<int>(sin(launchAngle) * speed);
}

void EnemyProjectile::Damage(const std::shared_ptr<Area>& area)
{
    auto playerPtr = player.lock();
    if (!playerPtr || !playerPtr->IsAlive())
    {
        return;
    }

    auto projectileCenteredPos = GetCenteredPosition();
    auto playerPos = playerPtr->GetCenteredPosition();
    float distance = projectileCenteredPos.distance(playerPos);
    
    if (distance < static_cast<float>(size) / 2.f)
    {
        playerPtr->Damage(damagePerHit);
        Terminate(); // Projectile hits player and is destroyed
    }
}

pdcpp::Point<int> EnemyProjectile::GetCenteredPosition() const
{
    return {static_cast<int>(position.x + size / 2), static_cast<int>(position.y + size / 2)};
}
