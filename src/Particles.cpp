//
// Created by Sergio Prada on 8/02/26.
//

#include "Particles.h"

#include "Globals.h"
#include "Log.h"
#include "pdcpp/graphics/Graphics.h"

Particles::Particles()
{
    lifetime = 0;
    iPositions = std::vector<pdcpp::Point<int>>(Globals::MAX_PARTICLES_PER_INSTANCE, pdcpp::Point<int>(0,0));
    fPositions = std::vector<pdcpp::Point<int>>(Globals::MAX_PARTICLES_PER_INSTANCE, pdcpp::Point<int>(0,0));
    velocities = std::vector<pdcpp::Point<float>>(Globals::MAX_PARTICLES_PER_INSTANCE, pdcpp::Point<float>(0,0));
}

void Particles::instantiate(pdcpp::Point<int> initialPosition)
{
    if (lifetime > 0) return; // We return since particles are already being drawn
    lifetime = Globals::MAX_PARTICLES_LIFETIME;
    for (auto& x : iPositions) x = initialPosition;  // Note the & for reference
    for (auto& x : fPositions) x = initialPosition;  // Note the & for reference
    for (int i = 0; i < velocities.size(); i++)
    {
        velocities[i].x = (rand() % 20 - 10) / 5.0f;  // Random -2 to 2
        velocities[i].y = (rand() % 20 - 10) / 5.0f;  // Random -2 to 2
    }
}

void Particles::update()
{
    if (lifetime <= 0) return;
    lifetime--;
    for (int i = 0; i < fPositions.size(); i++)
    {
        // Save old position for trail
        iPositions.at(i) = fPositions.at(i);

        // Apply velocity
        fPositions.at(i).x += velocities.at(i).x;
        fPositions.at(i).y += velocities.at(i).y;

        // Apply gravity
        velocities.at(i).y += 0.5;  // Gravity constant

        // Apply drag/friction
        velocities.at(i).x *= 0.98;
        velocities.at(i).y *= 0.98;
    }
}

void Particles::draw() const
{
    if (lifetime <= 0) return;
    for (int i = 0; i < iPositions.size(); i++)
    {
        pdcpp::Graphics::drawLine(iPositions.at(i), fPositions.at(i), 2, kColorWhite);
    }
}
