//
// Created by Sergio Prada on 26/12/24.
//

#ifndef CARDOBLAST_MAGIC_H
#define CARDOBLAST_MAGIC_H


#include "pdcpp/graphics/Point.h"

class Magic{
public:
    Magic() = delete;
    explicit Magic(pdcpp::Point<int> Position);
    bool operator==(const Magic& other) const {return this == &other;}

    void Update();
    virtual void Draw() const;
    virtual void Terminate();
    virtual void HandleInput();

    [[nodiscard]] bool IsAlive() const{return isAlive;}

protected:
    bool isAlive = false;
    unsigned int bornTime;
    unsigned int iLifetime; //milliseconds
    unsigned int elapsedTime;
    pdcpp::Point<int> position;
};


#endif //CARDOBLAST_MAGIC_H
