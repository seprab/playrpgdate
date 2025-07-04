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
    std::vector<Layer> mapData;
    std::shared_ptr<MapCollision> collider;
    pdcpp::ImageTable* imageTable = nullptr;
    int tokens{};
    int width{};
    int height{};
    int tileWidth{};
    int tileHeight{};
    char* dataPath = nullptr;
    char* tilesetPath = nullptr;
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


public:
    Area() = default;
    Area(const Area& other);
    Area(Area&& other) noexcept;
    Area(unsigned int _id, char* _name, char* _dataPath, int _dataTokens, char* _tilesetPath, std::shared_ptr<Dialogue> _dialogue, const std::vector<std::shared_ptr<Monster>>& _monsters);

    [[nodiscard]] int GetTokenCount() const {return tokens;}
    [[nodiscard]] char* GetDataPath() const {return dataPath;}
    [[nodiscard]] char* GetTilesetPath() const {return tilesetPath;}
    [[nodiscard]] std::vector<std::shared_ptr<Monster>> GetCreatures() const {return livingMonsters;}

    void SetTokenCount(int value){tokens=value;}
    void SetDataPath(char* value){dataPath=value;}
    void SetTilesetPath(char* value){tilesetPath=value;}
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;

    pdcpp::ImageTable* GetImageTable() {return imageTable;}
    void LoadLayers(const char* fileName, int limitOfTokens);
    void LoadImageTable(const char* fileName);
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

    [[nodiscard]] std::shared_ptr<AStarContainer> GetPathfindingContainer() const {return pathfindingContainer;}
    [[nodiscard]] std::vector<std::shared_ptr<Door>> GetDoors() const {return doors;}

    [[nodiscard]] int GetWidth() const {return width;};
    [[nodiscard]] int GetHeight() const {return height;};
    [[nodiscard]] int GetTileWidth() const {return tileWidth;};
    [[nodiscard]] int GetTileHeight() const {return tileHeight;};
    [[nodiscard]] MapCollision* GetCollider() const {return collider.get();}
};

#endif