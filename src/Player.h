#ifndef PLAYER_H
#define PLAYER_H

#include <unordered_set>
#include <string>
#include "Creature.h"
#include "AnimationClip.h"
#include "Projectile.h"

class EntityManager;

class Player final : public Creature
{
private:
    std::string className;
    unsigned int level;
    std::unordered_set<std::string> visitedAreas;

    // Animation clips
    std::unique_ptr<AnimationClip> idle{};
    std::unique_ptr<AnimationClip> run{};
    std::unique_ptr<AnimationClip> attack{};
    std::unique_ptr<AnimationClip> stab{};
    std::unique_ptr<AnimationClip> die{};

    int dx =0;
    int dy =0;

    bool attackingA;
    bool attackingB;
    unsigned int magicCooldown;
    unsigned int lastMagicCastTime;
    std::vector<std::unique_ptr<Magic>> magicLaunched;
public:
    Player();
    Player(const Player& other) = delete;
    Player(std::string _name, int _hp, int _strength, int _agility, double _evasion, unsigned int _xp, unsigned int _level, std::string _className);

    void Tick(const std::shared_ptr<Area>& area);
    void Move(int deltaX, int deltaY, const std::shared_ptr<Area>& area);
    void HandleInput();
    void Draw();

    std::string GetClassName();
    unsigned int GetLevel();
    std::unordered_set<std::string>& GetVisitedArea();

    unsigned int GetXPToLevelUp(unsigned int level);
    bool LevelUp();

    void Save(EntityManager* manager);
};

#endif