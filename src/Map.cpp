//
// Created by Sergio Prada on 9/12/24.
//
#include <cmath>
#include <pd_api/pd_api_file.h>
#include "Map.h"
#include "Utils.h"
#include "Log.h"
#include "pdcpp/graphics/ImageTable.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

void Map::LoadLayers(const char *fileName, int limitOfTokens)
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

    //validation
    int totalLayers = mapData[0].tiles.size();
    for (auto layer : mapData)
    {
        if (layer.tiles.size() != totalLayers)
        {
            Log::Error("Map layers have different sizes");
        }
    }
    height = width = sqrt(totalLayers);
}
void Map::LoadImageTable(const char *fileName)
{
    imageTable = new pdcpp::ImageTable(fileName);
    LCDBitmap* bitmap = (*imageTable)[0];
    pdcpp::GlobalPlaydateAPI::get()->graphics->getBitmapData(bitmap, &tileSize, nullptr, nullptr, nullptr, nullptr);
}
void Map::DrawTileFromLayer(int layer, int x, int y)
{
    int tileId = mapData[layer].tiles[(x * width) + y].id;
    LCDBitmap* bitmap = (*imageTable)[tileId];
    int drawX = x * tileSize*scale;
    int drawY = y * tileSize*scale;
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawScaledBitmap(bitmap, drawX, drawY, scale, scale);
}
void Map::Render(int x, int y, int fov)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            bool visibleX = abs(x-(i*tileSize*scale)) < fov;
            bool visibleY = abs(y-(j*tileSize)*scale) < fov;
            if (visibleX && visibleY)
            {
                DrawTileFromLayer(2, i, j);
                /*
                for (int k = 0; k < mapData.size(); k++)
                {
                    DrawTileFromLayer(k, i, j);
                }
                 */
            }
        }
    }
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
