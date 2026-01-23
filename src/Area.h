#ifndef AREA_H
#define AREA_H

#include "Inventory.h"
#include "Dialogue.h"
#include "AStarContainer.h"
#include "Globals.h"
#include "MapCollision.h"
#include "pdcpp/core/Random.h"
#include "pdcpp/graphics/ImageTable.h"
#include <memory>
#include <vector>

class EntityManager;
class Door;
class Monster;
class Player;
class EnemyProjectile;
class UI;

struct Tile {
    int id;
    bool collision;
};

struct Layer {
    std::vector<Tile> tiles;
};

class Area final : public Entity
{
private:
    EntityManager* entityManager = nullptr;
    std::vector<Layer> mapData;
    std::shared_ptr<MapCollision> collider;
    std::unique_ptr<pdcpp::ImageTable> imageTable;
    int width{};
    int height{};
    int tileWidth{};
    int tileHeight{};
    int ticksSinceLastSpawn = 0; // ticks since the last monster spawn
    int monstersSpawnedCount = 0; // total count of monsters that have been spawned (alive or dead)
    std::shared_ptr<Dialogue> dialogue;
    std::vector<std::shared_ptr<Door>> doors;
    std::vector<std::shared_ptr<Monster>> bankOfMonsters; // the type of monsters to spawn in the area
    std::vector<std::shared_ptr<Monster>> livingMonsters; // the monsters that are currently alive in the area
    std::vector<std::shared_ptr<Monster>> toSpawnMonsters; // the monsters that haven't been spawned yet
    std::shared_ptr<AStarContainer> pathfindingContainer;
    std::vector<std::unique_ptr<EnemyProjectile>> enemyProjectiles; // enemy projectiles in the area
    void SpawnCreature();
    [[nodiscard]] Map_Layer ToMapLayer() const;
    pdcpp::Random random = {};
    std::vector<pdcpp::Point<int>> spawnablePositions; // positions where monsters can spawn
    int pathfindingTickCounter = 0; // Counter to stagger pathfinding updates
    const int staggerAmount = Globals::MONSTER_MAX_LIVING_COUNT; // Number of groups to stagger
    bool isProcedural = false; // Flag to indicate if map is procedurally generated

    // Player activity tracking for slowdown ability
    bool playerIsActive = true;
    float playerIdleTime = 0.0f;
    unsigned int lastActivityCheckTime = 0;
    unsigned int lastSlowdownActivationTime = 0;
    bool slowdownActive = false;
    const unsigned int SLOWDOWN_COOLDOWN = 10000; // 10 seconds in milliseconds

public:
    Area();
    ~Area(); // Need explicit destructor for unique_ptr with forward declaration
    Area(const Area& other);
    Area(Area&& other) noexcept;
    Area(unsigned int _id, const char* _name, std::shared_ptr<Dialogue> _dialogue, const std::vector<std::shared_ptr<Monster>>& _monsters);

    [[nodiscard]] std::vector<std::shared_ptr<Monster>> GetCreatures() const {return livingMonsters;}
    [[nodiscard]] std::vector<std::shared_ptr<Monster>> GetMonsterBank() const {return bankOfMonsters;}
    [[nodiscard]] int GetMonstersSpawnedCount() const {return monstersSpawnedCount;}
    void AddLivingMonster(const std::shared_ptr<Monster>& monster) {livingMonsters.push_back(monster);}
    void ClearLivingMonsters() {livingMonsters.clear();}
    void ClearToSpawnMonsters() {toSpawnMonsters.clear();}
    void SetMonstersSpawnedCount(int count) {monstersSpawnedCount = count;}

    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager) override;

    void LoadLayers(std::string fileName, int limitOfTokens);
    void LoadImageTable(std::string fileName);
    void GenerateProceduralMap(int width = 40, int height = 40, UI* ui = nullptr);
    void DrawTileFromLayer(int layer, int x, int y);
    void Render(int x, int y, int fovX, int fovY);
    bool CheckCollision(int x, int y) const;
    void Tick(Player* player);
    void SetUpPathfindingContainer();
    void Unload();
    void SetupMonstersToSpawn();
    void LoadWithUI(UI* ui); // Load with UI for progress reporting
    void LoadFromSavedData(); // Load when map data was deserialized from save
    pdcpp::Point<int> FindSpawnablePosition(int attemptCount);
    void LoadSpawnablePositions();
    std::string SerializeMapData() const;
    bool DeserializeMapData(const std::string& jsonData);

    // Player activity tracking for enemy slowdown
    [[nodiscard]] bool GetPlayerActivityStatus() const { return playerIsActive; }
    [[nodiscard]] float GetPlayerIdleTime() const { return playerIdleTime; }
    [[nodiscard]] bool IsSlowdownActive() const { return slowdownActive; }

    [[nodiscard]] std::shared_ptr<AStarContainer> GetPathfindingContainer() const {return pathfindingContainer;}
    [[nodiscard]] std::vector<std::shared_ptr<Door>> GetDoors() const {return doors;}

    [[nodiscard]] int GetWidth() const {return width;};
    [[nodiscard]] int GetHeight() const {return height;};
    [[nodiscard]] int GetTileWidth() const {return tileWidth;};
    [[nodiscard]] int GetTileHeight() const {return tileHeight;};
    [[nodiscard]] MapCollision* GetCollider() const {return collider.get();}

    void SetEntityManager(EntityManager* manager) { entityManager = manager; }
    
    // Enemy projectile management
    void CreateEnemyProjectile(pdcpp::Point<int> position, float angle, float speed, unsigned int size, float damage);
    void AddEnemyProjectile(std::unique_ptr<EnemyProjectile> projectile);
    void UpdateEnemyProjectiles(Player* player);
    void DrawEnemyProjectiles() const;
};

#endif