//
// Created by Sergio Prada on 26/12/24.
//

#include "Magic.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"


Magic::Magic(pdcpp::Point<int> Position):
position(Position)
{
    isAlive = true;
    elapsedTime = 0;
    bornTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
}

void Magic::Update(const std::shared_ptr<Area>& area)
{
    if(!isAlive) return;

    unsigned int currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    elapsedTime = currentTime - bornTime;
    if (elapsedTime > iLifetime)
    {
        Terminate();
    }
    HandleInput();
    Draw();
    Damage(area);
}
void Magic::Terminate()
{
    isAlive = false;
}
void Magic::HandleInput()
{

}
void Magic::Draw() const
{

}
void Magic::Damage(const std::shared_ptr<Area>& area)
{
    // Implement damage logic here
}