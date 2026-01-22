//
// Created for enemy ranged attacks
//

#ifndef CARDOBLAST_ENEMYPROJECTILE_H
#define CARDOBLAST_ENEMYPROJECTILE_H

#include "pdcpp/graphics/Point.h"
#include "Magic.h"

class Player;

class EnemyProjectile : public Magic{
public:
    EnemyProjectile() = delete;
    EnemyProjectile(pdcpp::Point<int> Position, std::weak_ptr<Player> player, float angle, float speed, unsigned int size, float damage);
    bool operator==(const EnemyProjectile& other) const {return this == &other;}

    void Draw() const override;
    void HandleInput() override;
    void Damage(const std::shared_ptr<Area>& area) override;
    pdcpp::Point<int> GetCenteredPosition() const;

private:
    float speed;
    unsigned int size;
    float launchAngle; // Angle in radians
    float damagePerHit;
};

#endif //CARDOBLAST_ENEMYPROJECTILE_H
