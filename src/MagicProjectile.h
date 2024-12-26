//
// Created by Sergio Prada on 26/12/24.
//

#ifndef CARDOBLAST_MAGICPROJECTILE_H
#define CARDOBLAST_MAGICPROJECTILE_H

#include <memory>
#include "pdcpp/graphics/Point.h"


class MagicProjectile
        {
public:
    MagicProjectile() = delete;
    MagicProjectile(pdcpp::Point<int> Position);
    bool operator==(const MagicProjectile& other) const {return this == &other;}

    void Update();
    void Draw() const;
    void Kill();
    void HandleInput();
    pdcpp::Point<int> GetPosition();
    bool IsAlive() const{return isAlive;}

private:
    bool isAlive = false;
    bool exploding = false;
    pdcpp::Point<int> position;
    float speed {8};
    unsigned int travelSize {10};
    unsigned int explosionSize {50};
    unsigned int currentSize;
    unsigned int explosionThreshold {500};
    unsigned int iLifetime {2000}; //in milliseconds
    unsigned int bornTime;
};


#endif //CARDOBLAST_MAGICPROJECTILE_H
