//
// Created by Sergio Prada on 16/06/24.
//

#include "Player.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "Log.h"
#include "Beam.h"
#include "Globals.h"
#include "Projectile.h"
#include "OrbitingProjectiles.h"
#include "AutoProjectile.h"
#include "Monster.h"
#include "Area.h"
#include <limits>
#include <algorithm>

Player::Player(): Creature(0, "Player", "", 100, 10, 5, 5, 0.1, 0, 0, 0), level(0)
{
    ResetStats();
    className = "Warrior";
    SetHP(100);
    SetMaxHP(100);
    SetMovementScale(5.0f);
    lastAutoFireTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    lastActivityTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    isPlayerActive = true;

    idle = std::make_unique<AnimationClip>();
    run = std::make_unique<AnimationClip>();
    attack = std::make_unique<AnimationClip>();
    stab = std::make_unique<AnimationClip>();
    die = std::make_unique<AnimationClip>();

    animationTable = std::make_unique<pdcpp::ImageTable>("images/player/animmini");
    auto& anim = *animationTable;  // Reference to the ImageTable
    SetSize(pdcpp::Point<int>(Globals::PLAYER_SIZE, Globals::PLAYER_SIZE));
    // 16 x 16
    // 16 x 4 = 64
    // 16 x 5 = 80
    idle->AddBitmap(anim[0]);
    idle->AddBitmap(anim[1]);
    idle->SetDelay(4);
    idle->LoadBitmaps();

    run->AddBitmap(anim[4]);
    run->AddBitmap(anim[5]);
    run->AddBitmap(anim[6]);
    run->AddBitmap(anim[7]);
    run->SetDelay(3);
    run->LoadBitmaps();

    attack->AddBitmap(anim[8]);
    attack->AddBitmap(anim[9]);
    attack->AddBitmap(anim[10]);
    attack->AddBitmap(anim[11]);
    attack->SetDelay(0);
    attack->LoadBitmaps();

    stab->AddBitmap(anim[12]);
    stab->AddBitmap(anim[13]);
    stab->AddBitmap(anim[14]);
    stab->SetDelay(0);
    stab->LoadBitmaps();

    die->AddBitmap(anim[16]);
    die->AddBitmap(anim[17]);
    die->AddBitmap(anim[18]);
    die->SetDelay(4);
    die->LoadBitmaps();

    skills = {
            {"Beam", "images/ui/icon_magic_beam", 2000,
                [this](const pdcpp::Point<int>& position) { return std::make_unique<Beam>(position, weak_from_this()); }},
            {"Projectile", "images/ui/icon_magic_projectile", 1500,
                [this](const pdcpp::Point<int>& position) { return std::make_unique<Projectile>(position, weak_from_this()); }},
            {"Orbiting", "images/ui/icon_magic_orbiting_projectile", 5000,
                [this](const pdcpp::Point<int>& position) { return std::make_unique<OrbitingProjectiles>(position, weak_from_this()); }},
            {"Rapid Shot", "images/ui/icon_magic_projectile", 800,
                [this](const pdcpp::Point<int>& position) { return std::make_unique<Projectile>(position, weak_from_this(), 10.0f, 4, 500, 0.25f); }},
            {"Heavy Beam", "images/ui/icon_magic_beam", 3500,
                [this](const pdcpp::Point<int>& position) { return std::make_unique<Beam>(position, weak_from_this(), 3, 200.0f, 500, 0.7f); }}
    };
    lastSkillCastTimes.assign(skills.size(), pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds());
}

void Player::Tick(const std::shared_ptr<Area>& area)
{
    HandleInput();
    HandleAutoFire(area);
    Move(dx, dy, area);
    //Draw();

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
    if (CalculateFlashing())
    {
        if (attacking) attack->Draw(GetPosition().x, GetPosition().y);
        else if (dx != 0 || dy != 0) run->Draw(GetPosition().x, GetPosition().y);
        else idle->Draw(GetPosition().x, GetPosition().y);
    }
    Entity::DrawHealthBar();
    DrawAimDirection();
#if DEBUG
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(GetPosition().first, GetPosition().second, 16, 16, kColorWhite);
#endif
}
void Player::HandleInput()
{
    PDButtons clicked, pushed;
    pdcpp::GlobalPlaydateAPI::get()->system->getButtonState(&clicked, &pushed, nullptr);
    dx = 0, dy = 0;

    // Track player activity for enemy slowdown
    bool movementInput = (clicked & (kButtonUp | kButtonDown | kButtonLeft | kButtonRight));
    float crankChange = pdcpp::GlobalPlaydateAPI::get()->system->getCrankChange();
    bool crankRotating = (crankChange > 0.5f || crankChange < -0.5f);

    if (movementInput || crankRotating)
    {
        isPlayerActive = true;
        lastActivityTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    }
    else
    {
        // Check if player has been idle for more than 0.5 seconds
        unsigned int currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
        if (currentTime - lastActivityTime > 500) // 500ms threshold
        {
            isPlayerActive = false;
        }
    }
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

    if (skills.empty())
    {
        return;
    }

    if (pushed & kButtonA)
    {
        selectedMagic = selectedMagic + 1;
        if (selectedMagic >= skills.size())
        {
            selectedMagic = 0;
        }
    }
    else if (pushed & kButtonB)
    {
        if (selectedMagic == 0) selectedMagic = skills.size() - 1;
        else selectedMagic = selectedMagic - 1;
    }

    const auto currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    const int magicCastElapsedTime = static_cast<int>(currentTime - lastSkillCastTimes[selectedMagic]);
    if (magicCastElapsedTime < static_cast<int>(skills[selectedMagic].cooldownMs))
    {
        return;
    }

    if (attacking)
    {
        std::unique_ptr<Magic> magic;
        lastSkillCastTimes[selectedMagic] = currentTime;
        magic = skills[selectedMagic].factory(GetCenteredPosition());
        magicLaunched.push_back(std::move(magic));
    }
}
float Player::GetCooldownPercentage() const
{
    if (skills.empty())
    {
        return 1.0f;
    }
    unsigned int magicCastElapsedTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds()
        - lastSkillCastTimes[selectedMagic];
    float cooldown = static_cast<float>(skills[selectedMagic].cooldownMs);
    if (cooldown <= 0.0f)
    {
        return 1.0f;
    }
    return std::min(1.0f, static_cast<float>(magicCastElapsedTime) / cooldown);
}

std::string Player::GetClassName()
{
    return className;
}

unsigned int Player::GetLevel()
{
    return level;
}

void Player::SetSelectedMagic(unsigned int value)
{
    if (skills.empty())
    {
        selectedMagic = 0;
        return;
    }
    selectedMagic = value % static_cast<unsigned int>(skills.size());
}

std::unordered_set<std::string>& Player::GetVisitedArea()
{
    return visitedAreas;
}

unsigned int Player::GetXPToLevelUp(unsigned int currentLevel)
{
    return Globals::XP_BASE + (currentLevel * currentLevel * Globals::XP_FACTOR);
}

void Player::AddXP(unsigned int amount)
{
    SetXP(GetXP() + amount);
    while (GetXP() >= GetXPToLevelUp(level))
    {
        SetXP(GetXP() - GetXPToLevelUp(level));
        LevelUp();
    }
}

bool Player::SpendSkillPoint(StatType statType)
{
    if (skillPoints == 0)
    {
        return false;
    }

    switch (statType)
    {
        case StatType::Strength:
            SetStrength(GetStrength() + 1);
            break;
        case StatType::Agility:
            SetAgility(GetAgility() + 1);
            break;
        case StatType::Constitution:
            SetConstitution(GetConstitution() + 1);
            break;
        case StatType::MaxHp:
        {
            float newMax = GetMaxHP() + 5.0f;
            SetMaxHP(newMax);
            SetHP(std::min(newMax, GetHP() + 5.0f));
            break;
        }
    }

    skillPoints--;
    return true;
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

void Player::HandleAutoFire(const std::shared_ptr<Area>& area)
{
    const auto currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    const int autoFireElapsedTime = static_cast<int>(currentTime - lastAutoFireTime);

    if (autoFireElapsedTime < autoFireCooldown)
    {
        return;
    }

    // Find the closest enemy
    auto creatures = area->GetCreatures();
    if (creatures.empty())
    {
        return;
    }

    std::shared_ptr<Monster> closestEnemy = nullptr;
    float minDistance = std::numeric_limits<float>::max();
    pdcpp::Point<int> playerPos = GetCenteredPosition();

    for (const auto& monster : creatures)
    {
        if (!monster->IsAlive())
        {
            continue;
        }

        float distance = playerPos.distance(monster->GetCenteredPosition());
        if (distance < minDistance)
        {
            minDistance = distance;
            closestEnemy = monster;
        }
    }

    if (closestEnemy && minDistance <= Globals::AUTO_FIRE_RANGE)
    {
        // Launch auto-targeting projectile
        auto autoProjectile = std::make_unique<AutoProjectile>(GetCenteredPosition(), weak_from_this(), closestEnemy);
        magicLaunched.push_back(std::move(autoProjectile));
        lastAutoFireTime = currentTime;
    }
}

bool Player::LevelUp()
{
    level++;
    skillPoints++;
    SetHP(GetMaxHP());
    justLeveledUp = true; // Set flag to trigger level-up popup
    Log::Info("Leveled up to level %i, required exp to next level %i ", level, GetXPToLevelUp(level+1));
    return true;
}

void Player::Save(EntityManager* manager)
{
    //TODO: Implement player save
}

std::shared_ptr<void> Player::DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager) {
    return nullptr;
}

unsigned int Player::GetSurvivalTimeSeconds() const
{
    // If player is dead, return the final survival time
    if (!IsAlive() && finalSurvivalTime > 0)
    {
        return finalSurvivalTime;
    }

    // Otherwise, calculate current survival time
    unsigned int elapsedTime = pdcpp::GlobalPlaydateAPI::get()->system->getElapsedTime() + gameStartTime;
    return elapsedTime; // Convert to seconds
}

void Player::ResetStats()
{
    monstersKilled = 0;
    finalSurvivalTime = 0;
    gameStartTime = 0;
    level = 0;
    skillPoints = 0;
    SetXP(0);
}

void Player::Damage(float damage)
{
    bool wasAlive = IsAlive();

    // Call the base class Damage method
    Creature::Damage(damage);

    // If the player just died (was alive but now isn't), capture the final survival time
    if (wasAlive && !IsAlive() && finalSurvivalTime == 0)
    {
        finalSurvivalTime =  pdcpp::GlobalPlaydateAPI::get()->system->getElapsedTime() + gameStartTime; // Convert to seconds
    }
}
