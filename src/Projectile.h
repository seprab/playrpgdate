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
    explicit Projectile(pdcpp::Point<int> Position);
    bool operator==(const Projectile& other) const {return this == &other;}

    void Draw() const override;
    void HandleInput() override;

private:
    bool exploding = false;
    float speed;
    unsigned int size;
    unsigned int explosionThreshold;
};


#endif //CARDOBLAST_PROJECTILE_H
