//
// Created by Sergio Prada on 16/06/24.
//

#include "Player.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

Player::Player(): level(0)
{
    className = "Warrior";
    SetHP(100);
    SetMaxHP(100);
    SetStrength(10);
    SetAgility(5);
    SetEvasion(0.1);
    SetMovementScale(2.0f);

    idle = std::make_unique<AnimationClip>();
    run = std::make_unique<AnimationClip>();
    attack = std::make_unique<AnimationClip>();
    stab = std::make_unique<AnimationClip>();
    die = std::make_unique<AnimationClip>();

    idle->AddImagePath("images/player/idle_a.pdi");
    idle->AddImagePath("images/player/idle_b.pdi");
    idle->SetDelay(4);
    idle->LoadBitmaps();

    run->AddImagePath("images/player/run_a.pdi");
    run->AddImagePath("images/player/run_b.pdi");
    run->AddImagePath("images/player/run_c.pdi");
    run->AddImagePath("images/player/run_d.pdi");
    run->SetDelay(3);
    run->LoadBitmaps();

    attack->AddImagePath("images/player/attack_a.pdi");
    attack->AddImagePath("images/player/attack_b.pdi");
    attack->AddImagePath("images/player/attack_c.pdi");
    attack->AddImagePath("images/player/attack_d.pdi");
    attack->SetDelay(0);
    attack->LoadBitmaps();

    stab->AddImagePath("images/player/jab_a.pdi");
    stab->AddImagePath("images/player/jab_b.pdi");
    stab->AddImagePath("images/player/jab_c.pdi");
    stab->SetDelay(0);
    stab->LoadBitmaps();

    die->AddImagePath("images/player/death_a.pdi");
    die->AddImagePath("images/player/death_b.pdi");
    die->AddImagePath("images/player/death_c.pdi");
    die->SetDelay(4);
    die->LoadBitmaps();
}

void Player::Tick()
{
    PDButtons clicked;
    pdcpp::GlobalPlaydateAPI::get()->system->getButtonState(&clicked, nullptr, nullptr);

    int x = 0, y = 0;
    if (clicked & kButtonRight)
    {
        x=1;
        idle->SetFlip(false);
        run->SetFlip(false);
        attack->SetFlip(false);
        stab->SetFlip(false);
    }
    else if (clicked & kButtonLeft)
    {
        x= -1;
        idle->SetFlip(true);
        run->SetFlip(true);
        attack->SetFlip(true);
        stab->SetFlip(true);
    }
    if (clicked & kButtonUp) y=-1;
    else if (clicked & kButtonDown) y=1;
    x *= GetMovementScale();
    y *= GetMovementScale();
    SetPosition(std::pair<int,int>(GetPosition().first + x, GetPosition().second + y));


    if (clicked & kButtonA) attack->Draw(GetPosition().first, GetPosition().second);
    else if (clicked & kButtonB) stab->Draw(GetPosition().first, GetPosition().second);
    else if (x != 0 || y != 0) run->Draw(GetPosition().first, GetPosition().second);
    else idle->Draw(GetPosition().first, GetPosition().second);
}
