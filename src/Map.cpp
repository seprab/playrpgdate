//
// Created by Sergio Prada on 9/12/24.
//

#include "Map.h"

void Map::LoadFromFile(const char *filename)
{

}

void Map::Render() {

}

bool Map::CheckCollision(int x, int y)
{
    for (auto layer : mapData)
    {
        if (layer.tiles[x + y].collision)
        {
            return true;
        }
    }
    return false;
}
