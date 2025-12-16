#ifndef PLAYER_H
#define PLAYER_H

#include <unordered_set>
#include <string>
#include "Creature.h"
#include "AnimationClip.h"
#include "Magic.h"

class EntityManager;
class Area;

class Player : public Creature, public std::enable_shared_from_this<Player>
{
private:
    std::string className;
    unsigned int level;
    std::unordered_set<std::string> visitedAreas;

    std::unique_ptr<pdcpp::ImageTable> animationTable;
    // Animation clips
    std::unique_ptr<AnimationClip> idle{};
    std::unique_ptr<AnimationClip> run{};
    std::unique_ptr<AnimationClip> attack{};
    std::unique_ptr<AnimationClip> stab{};
    std::unique_ptr<AnimationClip> die{};

    int dx =0;
    int dy =0;

    bool attacking = true;
    unsigned int magicCooldown;
    unsigned int lastMagicCastTime;
    unsigned int selectedMagic = 0;
    std::vector<std::unique_ptr<Magic>> magicLaunched;
    std::vector<std::function<std::unique_ptr<Magic>(const pdcpp::Point<int>&)>> availableMagic;

    // Auto-fire passive ability
    unsigned int autoFireCooldown = 1000; // 1 second
    unsigned int lastAutoFireTime;

    // Game stats tracking
    unsigned int monstersKilled = 0;
    unsigned int gameStartTime = 0;
    unsigned int finalSurvivalTime = 0; // Captured when player dies

public:
    Player();
    Player(const Player& other) = delete;
    Player(std::string _name, int _hp, int _strength, int _agility, double _evasion, unsigned int _xp, unsigned int _level, std::string _className);

    void Tick(const std::shared_ptr<Area>& area);
    void Move(int deltaX, int deltaY, const std::shared_ptr<Area>& area);
    void HandleInput();
    void HandleAutoFire(const std::shared_ptr<Area>& area);
    void Draw() override;
    void DrawAimDirection() const;
    void Damage(float damage); // Override to capture final survival time

    float GetCooldownPercentage() const;
    std::string GetClassName();
    unsigned int GetLevel();
    std::unordered_set<std::string>& GetVisitedArea();

    unsigned int GetXPToLevelUp(unsigned int level);
    bool LevelUp();
    [[nodiscard]] unsigned int GetSelectedMagic() const { return selectedMagic; }
    void Save(EntityManager* manager);
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager) override;

    // Game stats
    void IncrementKillCount() { monstersKilled++; }
    [[nodiscard]] unsigned int GetMonstersKilled() const { return monstersKilled; }
    void SetGameStartTime(unsigned int time) { gameStartTime = time; }
    [[nodiscard]] unsigned int GetSurvivalTimeSeconds() const;
    void ResetStats();

};

#endif