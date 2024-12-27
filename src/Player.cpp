//
// Created by Sergio Prada on 16/06/24.
//

#include "Player.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"

Player::Player(): level(0)
{
    className = "Warrior";
    magicCooldown = 5000;
    SetHP(100);
    SetMaxHP(100);
    SetStrength(10);
    SetAgility(5);
    SetEvasion(0.1);
    SetMovementScale(5.0f);
    SetPosition(std::pair<int,int>(0, 0));

    idle = std::make_unique<AnimationClip>();
    run = std::make_unique<AnimationClip>();
    attack = std::make_unique<AnimationClip>();
    stab = std::make_unique<AnimationClip>();
    die = std::make_unique<AnimationClip>();

    auto anim =new pdcpp::ImageTable("images/player/animmini");

    // 16 x 16
    // 16 x 4 = 64
    // 16 x 5 = 80
    idle->AddBitmap((*anim)[0]);
    idle->AddBitmap((*anim)[1]);
    idle->SetDelay(4);
    idle->LoadBitmaps();

    run->AddBitmap((*anim)[4]);
    run->AddBitmap((*anim)[5]);
    run->AddBitmap((*anim)[6]);
    run->AddBitmap((*anim)[7]);
    run->SetDelay(3);
    run->LoadBitmaps();

    attack->AddBitmap((*anim)[8]);
    attack->AddBitmap((*anim)[9]);
    attack->AddBitmap((*anim)[10]);
    attack->AddBitmap((*anim)[11]);
    attack->SetDelay(0);
    attack->LoadBitmaps();

    stab->AddBitmap((*anim)[12]);
    stab->AddBitmap((*anim)[13]);
    stab->AddBitmap((*anim)[14]);
    stab->SetDelay(0);
    stab->LoadBitmaps();

    die->AddBitmap((*anim)[16]);
    die->AddBitmap((*anim)[17]);
    die->AddBitmap((*anim)[18]);
    die->SetDelay(4);
    die->LoadBitmaps();
}

void Player::Tick(const std::shared_ptr<Area>& area)
{
    HandleInput();
    Move(dx, dy, area);
    Draw();

    for(auto& magic : magicLaunched)
    {
        magic->Update();
        if (!magic->IsAlive())
        {
            magic.reset();
        }
    }
    magicLaunched.erase(std::remove(magicLaunched.begin(), magicLaunched.end(), nullptr), magicLaunched.end());
}

void Player::Move(int deltaX, int deltaY, const std::shared_ptr<Area>& area)
{

    if (deltaY == 0 && deltaX == 0) return;

    int x = GetPosition().first + deltaX;
    int y = GetPosition().second + deltaY;

    if (area->CheckCollision(x+8, y+10)) // Set the collision point to the player's feet
    {
        return;
    }
    SetPosition(std::pair<int,int>(x, y));
}
void Player::Draw()
{
    if (attackingA) attack->Draw(GetPosition().first, GetPosition().second);
    else if (attackingB) stab->Draw(GetPosition().first, GetPosition().second);
    else if (dx != 0 || dy != 0) run->Draw(GetPosition().first, GetPosition().second);
    else idle->Draw(GetPosition().first, GetPosition().second);

#if DEBUG
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(GetPosition().first, GetPosition().second, 16, 16, kColorWhite);
#endif
}
void Player::HandleInput()
{
    PDButtons clicked;
    pdcpp::GlobalPlaydateAPI::get()->system->getButtonState(&clicked, nullptr, nullptr);
    dx = 0, dy = 0;
    if (clicked & kButtonRight)
    {
        dx=1;
        idle->SetFlip(false);
        run->SetFlip(false);
        attack->SetFlip(false);
        stab->SetFlip(false);
    }
    else if (clicked & kButtonLeft)
    {
        dx= -1;
        idle->SetFlip(true);
        run->SetFlip(true);
        attack->SetFlip(true);
        stab->SetFlip(true);
    }
    if (clicked & kButtonUp) dy=-1;
    else if (clicked & kButtonDown) dy=1;
    dx *= GetMovementScale();
    dy *= GetMovementScale();



    int magicCastElapsedTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds() - lastMagicCastTime;
    if (magicCastElapsedTime < magicCooldown)
    {
        return;
    }

    attackingA = (clicked & kButtonA);
    attackingB = (clicked & kButtonB);

    if (attackingA)
    {
        lastMagicCastTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
        pdcpp::Point<int> Position = pdcpp::Point<int>(GetPosition().first, GetPosition().second);
        std::unique_ptr<Magic> projectile = std::make_unique<Projectile>(Position);
        magicLaunched.push_back(std::move(projectile));
    }
}