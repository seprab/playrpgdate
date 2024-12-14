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
        if (t[i].type == JSMN_STRING)
        {
            char* bufferValue = Utils::Subchar(charBuffer, t[i].start, t[i].end);
            if (strcmp(bufferValue, "data") == 0)
            {
                i=i+1;
                Layer layer;
                int numberOfTiles = t[i].size;
                for (int j = 1; j < numberOfTiles; j++)
                {
                    char* bufferValue = Utils::Subchar(charBuffer, t[i+j].start, t[i+j].end);
                    int parsedId = atoi(bufferValue);
                    Tile tile{.id =  parsedId, .collision =  true};
                    layer.tiles.push_back(tile);
                }
                layer.tiles[0].collision = false; //Since I'm using the first tile for drawing the background, I don't want it to be a collision
                mapData.push_back(layer);
                i+=t[i].size;
            }
        }
    }
    height = atoi(Utils::ValueDecoder(charBuffer, t, 0, t[0].end, "height"));
    width = atoi(Utils::ValueDecoder(charBuffer, t, 0, t[0].end, "width"));
    Log::Info("Map loaded, %i width and %i height", width, height);
}
void Map::LoadImageTable(const char *fileName)
{
    imageTable = new pdcpp::ImageTable(fileName);
    LCDBitmap* bitmap = (*imageTable)[0];
    pdcpp::GlobalPlaydateAPI::get()->graphics->getBitmapData(bitmap, &tileWidth, &tileHeight, nullptr, nullptr, nullptr);
}
void Map::DrawTileFromLayer(int layer, int x, int y)
{
    int tileId = mapData[layer].tiles[(y * width) + x].id;
    if (tileId>0) tileId = tileId - 1;

    LCDBitmap* bitmap = (*imageTable)[tileId];
    int drawX = x * tileWidth/*scale*/;
    int drawY = y * tileHeight/*scale*/;
    //pdcpp::GlobalPlaydateAPI::get()->graphics->drawScaledBitmap(bitmap, drawX, drawY, scale, scale);
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawBitmap(bitmap, drawX, drawY, LCDBitmapFlip::kBitmapUnflipped);
    //Log::Info("Tile at x: %i and y: %i uses ID: %i", x, y, tileId);
}
void Map::Render(int x, int y, int fovX, int fovY)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            bool visibleX = abs(x-(i*tileWidth/*scale*/)) < fovX;
            bool visibleY = abs(y-(j*tileHeight)/*scale*/) < fovY;
            if (visibleX && visibleY)
            {
                for (int k = 0; k < mapData.size(); k++)
                {
                    DrawTileFromLayer(k, i, j);
                }
            }
        }
    }
}

bool Map::CheckCollision(int x, int y)
{
    for (auto layer : mapData)
    {
        if (layer.tiles[(y * width) + x].collision)
        {
            return true;
        }
    }
    return false;
}
