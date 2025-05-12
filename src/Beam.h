//
// Created by Sergio Prada on 26/12/24.
//

#ifndef CARDOBLAST_BEAM_H
#define CARDOBLAST_BEAM_H

#include "pdcpp/graphics/Point.h"
#include "Magic.h"

class Beam : public Magic{
public:
    Beam() = delete;
    explicit Beam(pdcpp::Point<int> Position);
    bool operator==(const Beam& other) const {return this == &other;}

    void Draw() const override;
    void HandleInput() override;
    void Damage(const std::shared_ptr<Area>& area) override;

private:
    bool exploding = false;
    unsigned int size;
    unsigned int explosionThreshold;
    pdcpp::Point<int> startPosition;
    pdcpp::Point<int> endPosition;
};


#endif //CARDOBLAST_BEAM_H
