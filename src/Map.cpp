//
// Created by Sergio Prada on 9/12/24.
//

#include <pd_api/pd_api_file.h>
#include "Map.h"
#include "Utils.h"
#include "Log.h"

void Map::LoadFromFile(const char *fileName, int limitOfTokens)
{
    auto fileStat = new FileStat;
    const auto charBuffer = Utils::ReadBufferFromJSON(fileName, limitOfTokens, fileStat);
    auto* parser = new jsmn_parser;
    jsmn_init(parser);
    jsmntok_t t[limitOfTokens];
    int calculatedTokens = Utils::InitializeJSMN(parser, charBuffer, fileStat->size, limitOfTokens, t);

    for (int i=0; i<calculatedTokens; i++)
    {
        if (t[i].type == JSMN_ARRAY && t[i+1].type == JSMN_PRIMITIVE)
        {
            Layer layer;
            int numberOfTiles = t[i].size;
            for (int j = 1; j < numberOfTiles; j++)
            {
                char* bufferValue = Utils::Subchar(charBuffer, t[i+j].start, t[i+j].end);
                int parsedId = atoi(bufferValue);
                Tile tile{.id =  parsedId, .collision =  false};
                layer.tiles.push_back(tile);
            }
            mapData.push_back(layer);
            i+=t[i].size;
        }
    }
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
