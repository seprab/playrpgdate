#ifndef AREA_H
#define AREA_H

#include "Entity.h"
#include "Inventory.h"
#include "Creature.h"
#include "Dialogue.h"
#include "pdcpp/graphics/ImageTable.h"

class EntityManager;
class Door;
class Creature;

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
    pdcpp::ImageTable* imageTable;
    int tokens{};
    int width{};
    int height{};
    int tileWidth{};
    int tileHeight{};
    char* dataPath;
    char* tilesetPath;
    std::shared_ptr<Dialogue> dialogue;
    std::vector<std::shared_ptr<Door>> doors;
    std::vector<std::shared_ptr<Creature>> creatures;
    void SpawnCreatures();

public:
    Area() = default;
    Area(const Area& other);
    Area(Area&& other) noexcept;
    Area(unsigned int _id, char* _name, char* _dataPath, int _dataTokens, char* _tilesetPath, std::shared_ptr<Dialogue> _dialogue, std::vector<std::shared_ptr<Creature>> _creatures);

    [[nodiscard]] int GetTokenCount() const {return tokens;}
    [[nodiscard]] char* GetDataPath() const {return dataPath;}
    [[nodiscard]] char* GetTilesetPath() const {return tilesetPath;}
    [[nodiscard]] std::vector<std::shared_ptr<Creature>> GetCreatures() const {return creatures;}

    void SetTokenCount(int value){tokens=value;}
    void SetDataPath(char* value){dataPath=value;}
    void SetTilesetPath(char* value){tilesetPath=value;}
    std::shared_ptr<void> DecodeJson(char *buffer, jsmntok_t *tokens, int size) override;

    pdcpp::ImageTable* GetImageTable() {return imageTable;}
    void LoadLayers(const char* fileName, int limitOfTokens);
    void LoadImageTable(const char* fileName);
    void DrawTileFromLayer(int layer, int x, int y);
    void Render(int x, int y, int fovX, int fovY);
    bool CheckCollision(int x, int y);
    void Tick();

    void Load();
    void Unload();

    [[nodiscard]] int GetWidth() const {return width;};
    [[nodiscard]] int GetHeight() const {return height;};
    [[nodiscard]] int GetTileWidth() const {return tileWidth;};
    [[nodiscard]] int GetTileHeight() const {return tileHeight;};

};

#endif