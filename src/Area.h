#ifndef AREA_H
#define AREA_H

#include "Inventory.h"
#include "Dialogue.h"
#include "AStarContainer.h"
#include "Globals.h"
#include "MapCollision.h"
#include "pdcpp/core/Random.h"
#include "pdcpp/graphics/ImageTable.h"

class EntityManager;
class Door;
class Monster;
class Player;

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
    int tokens{};
    int width{};
    int height{};
    int tileWidth{};
    int tileHeight{};
    std::string dataPath;
    std::string tilesetPath;
    int ticksSinceLastSpawn = 0; // ticks since the last monster spawn
    std::shared_ptr<Dialogue> dialogue;
    std::vector<std::shared_ptr<Door>> doors;
    std::vector<std::shared_ptr<Monster>> bankOfMonsters; // the type of monsters to spawn in the area
    std::vector<std::shared_ptr<Monster>> livingMonsters; // the monsters that are currently alive in the area
    std::vector<std::shared_ptr<Monster>> toSpawnMonsters; // the monsters that haven't been spawned yet
    std::shared_ptr<AStarContainer> pathfindingContainer;
    void SpawnCreature();
    [[nodiscard]] Map_Layer ToMapLayer() const;
    pdcpp::Random random = {};
    std::vector<pdcpp::Point<int>> spawnablePositions; // positions where monsters can spawn
    int pathfindingTickCounter = 0; // Counter to stagger pathfinding updates
    const int staggerAmount = Globals::MONSTER_MAX_LIVING_COUNT; // Number of groups to stagger

    // Player activity tracking for slowdown ability
    bool playerIsActive = true;
    float playerIdleTime = 0.0f;
    unsigned int lastActivityCheckTime = 0;
    unsigned int lastSlowdownActivationTime = 0;
    bool slowdownActive = false;
    const unsigned int SLOWDOWN_COOLDOWN = 10000; // 10 seconds in milliseconds

public:
    Area() = default;
    Area(const Area& other);
    Area(Area&& other) noexcept;
    Area(unsigned int _id, const char* _name, const std::string& _dataPath, int _dataTokens, const std::string& _tilesetPath, std::shared_ptr<Dialogue> _dialogue, const std::vector<std::shared_ptr<Monster>>& _monsters);

    [[nodiscard]] int GetTokenCount() const {return tokens;}
    [[nodiscard]] const char* GetDataPath() const {return dataPath.c_str();}
    [[nodiscard]] const char* GetTilesetPath() const {return tilesetPath.c_str();}
    [[nodiscard]] std::vector<std::shared_ptr<Monster>> GetCreatures() const {return livingMonsters;}

    void SetTokenCount(int value){tokens=value;}
    void SetDataPath(const std::string& value){dataPath=value;}
    void SetTilesetPath(const std::string& value){tilesetPath=value;}
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size, EntityManager* entityManager) override;

    void LoadLayers(std::string fileName, int limitOfTokens);
    void LoadImageTable(std::string fileName);
    void DrawTileFromLayer(int layer, int x, int y);
    void Render(int x, int y, int fovX, int fovY);
    bool CheckCollision(int x, int y) const;
    void Tick(Player* player);
    void SetUpPathfindingContainer();
    void Load();
    void Unload();
    void SetupMonstersToSpawn();
    pdcpp::Point<int> FindSpawnablePosition(int attemptCount);
    void LoadSpawnablePositions();

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
};

#endif