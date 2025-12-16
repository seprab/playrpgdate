//
// Created by Sergio Prada on 16/12/24.
//

#ifndef CARDOBLAST_AUTOPROJECTILE_H
#define CARDOBLAST_AUTOPROJECTILE_H

#include "pdcpp/graphics/Point.h"
#include "Magic.h"

class Monster;

class AutoProjectile : public Magic{
public:
    AutoProjectile() = delete;
    explicit AutoProjectile(pdcpp::Point<int> Position, std::weak_ptr<Player> player, std::weak_ptr<Monster> target);
    bool operator==(const AutoProjectile& other) const {return this == &other;}

    void Draw() const override;
    void HandleInput() override;
    void Damage(const std::shared_ptr<Area>& area) override;
    pdcpp::Point<int> GetCenteredPosition() const;

private:
    std::weak_ptr<Monster> target;
    float speed;
    unsigned int size;
};


#endif //CARDOBLAST_AUTOPROJECTILE_H
