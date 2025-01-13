//
// Created by Sergio Prada on 9/01/25.
//

#ifndef CARDOBLAST_ORBITINGPROJECTILES_H
#define CARDOBLAST_ORBITINGPROJECTILES_H

#include "pdcpp/graphics/Point.h"
#include "Magic.h"
#include "pdcpp/core/util.h"

class OrbitingProjectiles : public Magic{
public:
    OrbitingProjectiles() = delete;
    explicit OrbitingProjectiles(pdcpp::Point<int> Position);
    bool operator==(const OrbitingProjectiles& other) const {return this == &other;}

    void Draw() const override;
    void HandleInput() override;

private:
    unsigned int size;
    short int radius;
    float angles[4] = {0.f, kPI/2.f, kPI, 3.f*kPI/2.f};
};


#endif //CARDOBLAST_ORBITINGPROJECTILES_H
