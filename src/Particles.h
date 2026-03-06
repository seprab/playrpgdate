//
// Created by Sergio Prada on 8/02/26.
//

#ifndef CARDOBLAST_PARTICLES_H
#define CARDOBLAST_PARTICLES_H

#include "pdcpp/graphics/AnimatedText.h"

class Particles
{
public:
    Particles();
    ~Particles() = default;

    void update();
    void draw() const;
    void instantiate(pdcpp::Point<int> initialPosition);
private:
    std::vector<pdcpp::Point<int>> iPositions;
    std::vector<pdcpp::Point<int>> fPositions;
    std::vector<pdcpp::Point<float>> velocities;
    int lifetime;
};


#endif //CARDOBLAST_PARTICLES_H