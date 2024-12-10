//
// Created by Sergio Prada on 9/12/24.
//
#include <vector>

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


public:
    Map() = default;
    ~Map() = default;
    void LoadFromFile(const char* filename);
    void Render();
    bool CheckCollision(int x, int y);
};


#endif //CARDOBLAST_MAP_H
