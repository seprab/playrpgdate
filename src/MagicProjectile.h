//
// Created by Sergio Prada on 26/12/24.
//

#ifndef CARDOBLAST_MAGICPROJECTILE_H
#define CARDOBLAST_MAGICPROJECTILE_H

#include "pdcpp/graphics/point.h"


class MagicProjectile{
public:
    MagicProjectile() = delete;
    MagicProjectile(pdcpp::Point<int> Position, float Speed, int Size);
    void Update();
    void Draw();
    void Kill();
    void HandleInput();
    pdcpp::Point<int> GetPosition();
    float GetSize();

private:
    bool isAlive;
    pdcpp::Point<int> position;
    float speed;
    int size;
    unsigned int iLifetime; //in milliseconds
    unsigned int bornTime;
};


#endif //CARDOBLAST_MAGICPROJECTILE_H
