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
    int tileSize{};
    float scale{};

public:
    Map() = default;
    ~Map() = default;
    void LoadLayers(const char* fileName, int limitOfTokens);
    void LoadImageTable(const char* fileName);
    void DrawTileFromLayer(int layer, int x, int y);
    void Render(int x, int y, int fov);
    bool CheckCollision(int x, int y);
    void SetMapScale(float value){scale = value;};
};


#endif //CARDOBLAST_MAP_H
