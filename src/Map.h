//
// Created by Sergio Prada on 9/12/24.
//
#include <vector>
#include "pdcpp/graphics/ImageTable.h"

#ifndef CARDOBLAST_MAP_H
#define CARDOBLAST_MAP_H

struct Tile {
    int id;
    bool collision;
};

struct Layer {
    std::vector<Tile> tiles;
};

class Map{
private:
    std::vector<Layer> mapData;
    pdcpp::ImageTable* imageTable;
    int width{};
    int height{};
    int tileWidth{};
    int tileHeight{};
    float scale{};

public:
    Map() = default;
    ~Map() = default;
    pdcpp::ImageTable* GetImageTable() {return imageTable;}
    void LoadLayers(const char* fileName, int limitOfTokens);
    void LoadImageTable(const char* fileName);
    void DrawTileFromLayer(int layer, int x, int y);
    void Render(int x, int y, int fovX, int fovY);
    bool CheckCollision(int x, int y);
    void SetMapScale(float value){scale = value;};

    [[nodiscard]] int GetWidth() const {return width;};
    [[nodiscard]] int GetHeight() const {return height;};
    [[nodiscard]] int GetTileWidth() const {return tileWidth;};
    [[nodiscard]] int GetTileHeight() const {return tileHeight;};
};


#endif //CARDOBLAST_MAP_H
