//
// Created by Sergio Prada on 26/12/24.
//

#ifndef CARDOBLAST_PROJECTILE_H
#define CARDOBLAST_PROJECTILE_H

#include "pdcpp/graphics/Point.h"
#include "Magic.h"

class Projectile : public Magic{
public:
    Projectile() = delete;
    explicit Projectile(pdcpp::Point<int> Position, std::weak_ptr<Player> player);
    Projectile(pdcpp::Point<int> Position, std::weak_ptr<Player> player, float speed, unsigned int size, unsigned int explosionThreshold, float damage);
    bool operator==(const Projectile& other) const {return this == &other;}

    void Draw() const override;
    void HandleInput() override;
    void Damage(const std::shared_ptr<Area>& area) override;
    pdcpp::Point<int> GetCenteredPosition() const;

private:
    bool exploding = false;
    float speed;
    unsigned int size;
    unsigned int explosionThreshold;
    float launchAngle = 0.0f; // Angle in radians
    const int sizeIncrement = 3; // Increment size when exploding
    float damagePerHit = 0.5f;
};


#endif //CARDOBLAST_PROJECTILE_H
