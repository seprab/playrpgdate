//
// Created by Sergio Prada on 16/06/24.
//

#include "Player.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"
#include "Beam.h"
#include "Projectile.h"
#include "OrbitingProjectiles.h"

Player::Player(): Creature(0, "Player", "", 100, 10, 5, 5, 0.1, 0, 0, 0), level(0)
{
    className = "Warrior";
    magicCooldown = 5000;
    SetHP(100);
    SetMaxHP(100);
    SetMovementScale(5.0f);
    lastMagicCastTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();

    idle = std::make_unique<AnimationClip>();
    run = std::make_unique<AnimationClip>();
    attack = std::make_unique<AnimationClip>();
    stab = std::make_unique<AnimationClip>();
    die = std::make_unique<AnimationClip>();

    auto anim =new pdcpp::ImageTable("images/player/animmini");
    SetSize(pdcpp::Point<int>(16, 16));
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

    availableMagic = {
            [](const pdcpp::Point<int>& position) { return std::make_unique<Beam>(position); },
            [](const pdcpp::Point<int>& position) { return std::make_unique<Projectile>(position); },
            [](const pdcpp::Point<int>& position) { return std::make_unique<OrbitingProjectiles>(position); }
    };
}

void Player::Tick(const std::shared_ptr<Area>& area)
{
    HandleInput();
    Move(dx, dy, area);
    Draw();

    for(auto& magic : magicLaunched)
    {
        if (!magic->IsAlive())
        {
            magic.reset();
        }
        else
        {
            magic->Update(area);
        }
    }
    magicLaunched.erase(std::remove(magicLaunched.begin(), magicLaunched.end(), nullptr), magicLaunched.end());
}

void Player::Move(int deltaX, int deltaY, const std::shared_ptr<Area>& area)
{

    if (deltaY == 0 && deltaX == 0) return;

    int x = GetPosition().x + deltaX;
    int y = GetPosition().y + deltaY;

    if (area->CheckCollision(x+8, y+10)) // Set the collision point to the player's feet
    {
        return;
    }
    SetPosition(pdcpp::Point<int>(x, y));
}
void Player::Draw()
{
    if (attacking) attack->Draw(GetPosition().x, GetPosition().y);
    else if (dx != 0 || dy != 0) run->Draw(GetPosition().x, GetPosition().y);
    else idle->Draw(GetPosition().x, GetPosition().y);
    Entity::DrawHealthBar();
    DrawAimDirection();
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
    dx *= static_cast<int>(GetMovementSpeed());
    dy *= static_cast<int>(GetMovementSpeed());

    if (clicked & kButtonB)
    {
        selectedMagic++;
        if (selectedMagic >= availableMagic.size())
        {
            selectedMagic = 0;
        }
    }

    const auto currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    const int magicCastElapsedTime = static_cast<int>(currentTime - lastMagicCastTime);
    if (magicCastElapsedTime < magicCooldown)
    {
        return;
    }

    attacking = (clicked & kButtonA);
    if (attacking)
    {
        lastMagicCastTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
        std::unique_ptr<Magic> magic;
        magic = availableMagic[selectedMagic](GetCenteredPosition());
        magicLaunched.push_back(std::move(magic));
    }
}
float Player::GetCooldownPercentage() const
{
    unsigned int magicCastElapsedTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds() - lastMagicCastTime;
    return static_cast<float>(magicCastElapsedTime) / static_cast<float>(magicCooldown);
}

void Player::DrawAimDirection() const {
    // Draw an arrow to know where the crank is pointing to, so the player knows where to aim the magic
    // In the range 0-360. Zero is pointing up, and the value increases as the crank moves clockwise
    const float angle = pdcpp::GlobalPlaydateAPI::get()->system->getCrankAngle() * kPI / 180.f;
    int radius = 30;
    const pdcpp::Point<int> pos = GetCenteredPosition();

    int x = pos.x + (radius * cos(angle));
    int y = pos.y + (radius * sin(angle));
    radius = radius - 3;
    int xa = pos.x + (radius * cos(angle-0.1f));
    int ya = pos.y + (radius * sin(angle-0.1f));
    int xb = pos.x + (radius * cos(angle+0.1f));
    int yb = pos.y + (radius * sin(angle+0.1f));

    pdcpp::GlobalPlaydateAPI::get()->graphics->drawLine(x, y, xa, ya, 1, kColorWhite);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawLine(x, y, xb, yb, 1, kColorWhite);
}

bool Player::LevelUp()
{
    //TODO: Implement player level up
    level++;
    SetHP(GetMaxHP());
    return true;
}

void Player::Save(EntityManager* manager)
{
    //TODO: Implement player save
}

std::shared_ptr<void> Player::DecodeJson(char *buffer, jsmntok_t *tokens, int size) {
    return nullptr;
}
