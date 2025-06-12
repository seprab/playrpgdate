//
// Created by Sergio Prada on 11/06/25.
//

#include "Monster.h"

void Monster::Tick(Player* player)
{
    pdcpp::Point<int> playerPosition = player->GetPosition();
    if (ShouldMove())
    {
        CalculatePath(playerPosition);
        Move(playerPosition);
    }
    if (ShouldAttack(playerPosition))
    {
        player->Damage(1);
    }
    Draw();
}

bool Monster::ShouldMove() {
    return false;
}

void Monster::CalculatePath(pdcpp::Point<int> target) {

}

void Monster::Move(pdcpp::Point<int> target) {

}

bool Monster::ShouldAttack(pdcpp::Point<int> target) {
    return GetPosition().distance(target) <= 5;
}
