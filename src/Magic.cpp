//
// Created by Sergio Prada on 26/12/24.
//

#include "Magic.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"


Magic::Magic(pdcpp::Point<int> Position, const char* iconBitmapPath):
position(Position)
{
    isAlive = true;
    elapsedTime = 0;
    bornTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    iconBitmap = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(iconBitmapPath, nullptr);
}

void Magic::Update()
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
}

void Magic::Terminate() {
    isAlive = false;
}

void Magic::HandleInput() {

}

void Magic::Draw() const {

}
