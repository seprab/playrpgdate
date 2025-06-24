#include <utility>
#include "Area.h"
#include "Door.h"
#include "Entity.h"
#include "Dialogue.h"
#include "Log.h"
#include "Utils.h"
#include "pdcpp/core/File.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "EntityManager.h"
#include "Monster.h"
#include "pdcpp/core/Random.h"

Area::Area(unsigned int _id, char* _name, char* _dataPath, int _dataTokens, char* _tilesetPath, std::shared_ptr<Dialogue> _dialogue, const std::vector<std::shared_ptr<Monster>>& _monsters)
: Entity(_id), tokens(_dataTokens), dataPath(_dataPath), tilesetPath(_tilesetPath), dialogue(std::move(_dialogue))
{
    SetName(_name);
    for (const std::shared_ptr<Monster>& monster : _monsters)
    {
        bankOfMonsters.push_back(monster);
    }
    Log::Info("Area created with id: %d, name: %s", _id, _name);
}
Area::Area(const Area &other)
        : Entity(other.GetId()), tokens(other.GetTokenCount()), dataPath(other.GetDataPath()), tilesetPath(other.GetTilesetPath()), dialogue(other.dialogue), bankOfMonsters(other.bankOfMonsters)
{
    SetName(other.GetName());
}
Area::Area(Area &&other) noexcept
        : Entity(other.GetId()), tokens(other.GetTokenCount()), dataPath(other.GetDataPath()), tilesetPath(other.GetTilesetPath()), dialogue(std::move(other.dialogue)), bankOfMonsters(std::move(other.bankOfMonsters))
{
    SetName(other.GetName());
}

std::shared_ptr<void> Area::DecodeJson(char *buffer, jsmntok_t *tokens, const int size)
{
    std::vector<Area> decodedAreas;
    for (int i = 0; i < size; i++)
    {
        if(tokens[i].type != JSMN_OBJECT ) continue;

        int decodedId{};
        int decodedTokens{};
        char* decodedName{};
        char* decodedData{};
        char* decodedTileset{};
        std::shared_ptr<Dialogue> decodedDialogue;
        std::vector<std::shared_ptr<Door>> decodedDoors{};
        std::vector<std::shared_ptr<Monster>> decodedCreatures;

        const int endOfObject = tokens[i].end; //get the end of the Area object
        decodedId = static_cast<int>(strtol(Utils::ValueDecoder(buffer, tokens, tokens[i-1].start, endOfObject, "id"), nullptr, 10));
        decodedName = Utils::ValueDecoder(buffer, tokens, tokens[i].start, endOfObject, "name");
        decodedData = Utils::ValueDecoder(buffer, tokens, tokens[i].start, endOfObject, "dataPath");
        decodedTokens = static_cast<int>(strtol(Utils::ValueDecoder(buffer, tokens, tokens[i].start, endOfObject, "dataTokens"), nullptr, 10));
        decodedTileset = Utils::ValueDecoder(buffer, tokens, tokens[i].start, endOfObject, "tileset");

        i++; //move into the first property of the Area object. Otherwise, the while is invalid
        while(tokens[i].end < endOfObject)
        {
            if(tokens[i].type != JSMN_STRING)
            {
                i++;
                continue;
            }
            char* parseProperty = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);
            if(strcmp(parseProperty, "dialogue") == 0)
            {
                i++; //move into the dialogue token
                decodedDialogue = std::make_shared<Dialogue>(buffer, tokens, i);
            }
            else if(strcmp(parseProperty, "doors") == 0)
            {
                i++; //move into the doors array token
                int numOfDoors = tokens[i].size;
                i++; //either move to the first element of the array or move to the next property
                if (numOfDoors == 0) continue;
                for (int j = 0; j < numOfDoors; j++)
                {
                    char* door = Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end);
                    int decodedDoor = std::stoi(door);
                    auto originalInstance = EntityManager::GetInstance()->GetEntity(decodedDoor);
                    if (originalInstance == nullptr)
                    {
                        Log::Error("Door with ID %d not found", decodedDoor);
                        continue;
                    }
                    decodedDoors.push_back(std::static_pointer_cast<Door>(originalInstance));
                }
                i=i+numOfDoors;
            }
            else if(strcmp(parseProperty, "creatures") == 0)
            {
                i++; //move into the creatures array token
                int numOfCreatures = tokens[i].size;
                i++; //either move to the first element of the array or move to the next property
                for (int j = 0; j < numOfCreatures; j++)
                {
                    int creatureId = std::stoi(Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end));
                    auto originalInstance = EntityManager::GetInstance()->GetEntity(creatureId);
                    if (originalInstance == nullptr)
                    {
                        Log::Error("Creature with ID %d not found", creatureId);
                        continue;
                    }
                    decodedCreatures.push_back(std::static_pointer_cast<Monster>(originalInstance));
                }
                i=i+numOfCreatures;
            }
            else i++;
        }

        decodedAreas.emplace_back(decodedId, decodedName, decodedData, decodedTokens, decodedTileset, decodedDialogue, decodedCreatures);
        i = endOfObject-1; //move back to the end of the Area object
    }
    return std::make_shared<std::vector<Area>>(decodedAreas);
}
void Area::LoadLayers(const char *fileName, int limitOfTokens)
{
    auto fileHandle = std::make_unique<pdcpp::FileHandle>(fileName, kFileRead);
    auto charBuffer = std::make_unique<char[]>(fileHandle->getDetails().size+1);
    fileHandle->read(charBuffer.get(), fileHandle->getDetails().size);
    auto parser = std::make_unique<jsmn_parser>();
    jsmn_init(parser.get());
    auto t = std::make_unique<jsmntok_t[]>(limitOfTokens);
    int calculatedTokens = Utils::InitializeJSMN(parser.get(), charBuffer.get(), fileHandle->getDetails().size, limitOfTokens, t.get());

    for (int i=0; i<calculatedTokens; i++)
    {
        if (t[i].type == JSMN_STRING)
        {
            char* bufferValue = Utils::Subchar(charBuffer.get(), t[i].start, t[i].end);
            if (strcmp(bufferValue, "data") == 0)
            {
                i=i+1;
                Layer layer;
                int numberOfTiles = t[i].size;
                for (int j = 0; j < numberOfTiles; j++)
                {
                    /*
                     * In the following line I am summing 1 to the index of the tile otherwise it tries to cast '[' (the start of the array)
                     * to integer, and it doesn't get to the end of the tile id.
                     */
                    bufferValue = Utils::Subchar(charBuffer.get(), t[i+j+1].start, t[i+j+1].end);
                    int parsedId = static_cast<int>(strtol(bufferValue, nullptr, 10));
                    Tile tile{.id =  parsedId, .collision = parsedId != 0};
                    layer.tiles.push_back(tile);
                }
                mapData.push_back(layer);
                i+=t[i].size;
            }
        }
    }
    height = static_cast<int>(strtol(Utils::ValueDecoder(charBuffer.get(), t.get(), 0, t[0].end, "height"), nullptr, 10));
    width = static_cast<int>(strtol(Utils::ValueDecoder(charBuffer.get(), t.get(), 0, t[0].end, "width"), nullptr, 10));
    Log::Info("Map loaded, %i width and %i height", width, height);
}
void Area::LoadImageTable(const char *fileName)
{
    imageTable = new pdcpp::ImageTable(fileName);
    LCDBitmap* bitmap = (*imageTable)[0];
    pdcpp::GlobalPlaydateAPI::get()->graphics->getBitmapData(bitmap, &tileWidth, &tileHeight, nullptr, nullptr, nullptr);
}
void Area::DrawTileFromLayer(int layer, int x, int y)
{
    int tileId = mapData[layer].tiles[(y * width) + x].id;
    if (tileId == 0) return; // skip empty tiles
    tileId = tileId - 1; //I'm not sure why.

    LCDBitmap* bitmap = (*imageTable)[tileId];
    int drawX = x * tileWidth;
    int drawY = y * tileHeight;
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawBitmap(bitmap, drawX, drawY, LCDBitmapFlip::kBitmapUnflipped);
}
void Area::Render(int x, int y, int fovX, int fovY)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            bool visibleX = abs(x-(i*tileWidth)) < fovX;
            bool visibleY = abs(y-(j*tileHeight)) < fovY;
            if (visibleX && visibleY)
            {
                DrawTileFromLayer(0, i, j); // The first layer is the only one to draw
            }
        }
    }
    for (const auto& monster : livingMonsters)
    {
        auto monsterPos = monster->GetPosition();
        bool visibleX = abs(x - monsterPos.x) < fovX;
        bool visibleY = abs(y - monsterPos.y) < fovY;
        if (!visibleX || !visibleY) continue;
        monster->Draw();
    }
}
bool Area::CheckCollision(int x, int y) const
{
    x=x/tileWidth;
    y=y/tileHeight;
    if (mapData[0].tiles[(y * width) + x].collision)
    {
#if DEBUG
        pdcpp::GlobalPlaydateAPI::get()->graphics->drawRect(x*tileWidth, y*tileHeight, Globals::MAP_TILE_SIZE, Globals::MAP_TILE_SIZE, kColorWhite);
#endif
        return true;
    }
    return false;
}
void Area::Load()
{
    LoadLayers(dataPath, tokens);
    LoadImageTable(tilesetPath);
    collider = std::make_shared<MapCollision>();
    collider->SetMap(ToMapLayer(), width, height);
    LoadSpawnablePositions();
    SetupMonstersToSpawn();
}
void Area::Unload()
{
    mapData.clear();
    delete imageTable;
}
void Area::SetupMonstersToSpawn()
{
    for (int i=0; i< Globals::MONSTER_TOTAL_TO_SPAWN; i++)
    {
        unsigned int randomIndex = random.next() % static_cast<unsigned int>(bankOfMonsters.size());
        auto monster = std::make_shared<Monster>(*bankOfMonsters[randomIndex]);
        toSpawnMonsters.push_back(monster);
    }
}
void Area::SpawnCreature()
{
    if (ticksSinceLastSpawn < Globals::TICKS_BETWEEN_MONSTER_SPAWNS)
    {
        ticksSinceLastSpawn++;
        return; // don't spawn monsters if the time hasn't passed
    }
    int monstersToSpawn = Globals::MONSTER_TOTAL_TO_SPAWN - static_cast<int>(livingMonsters.size());
    if (monstersToSpawn <= 0 || toSpawnMonsters.empty()) return; // don't spawn more monsters if the max count is reached
    if (livingMonsters.size() >= Globals::MONSTER_MAX_LIVING_COUNT) return; // don't spawn more monsters if the max count is reached

    std::shared_ptr<Monster> monster = toSpawnMonsters[0];
    livingMonsters.push_back(monster);
    livingMonsters.back()->LoadBitmap();
    livingMonsters.back()->SetTiledPosition(Area::FindSpawnablePosition()); // set a default position for the monster
    toSpawnMonsters.erase(toSpawnMonsters.begin());
    ticksSinceLastSpawn = 0; // reset the spawn timer
}
/// Find a spawnable position in the area, out of the sight of the player and not colliding with any tile.
pdcpp::Point<int> Area::FindSpawnablePosition()
{
    pdcpp::Point<int> playerPosition = EntityManager::GetInstance()->GetPlayer()->GetTiledPosition();
    unsigned int randomIndex = random.next() % static_cast<unsigned int>(spawnablePositions.size());
    if (playerPosition.distance(spawnablePositions[randomIndex]) < Globals::MONSTER_SPAWN_RADIUS)
    {
        // if the position is too close to the player, find another one
        return FindSpawnablePosition();
    }
    return spawnablePositions[randomIndex];
}
void Area::LoadSpawnablePositions()
{
    spawnablePositions = std::vector<pdcpp::Point<int>>();
    for (int i = 0; i < mapData[1].tiles.size(); i++)
    {
        if (mapData[1].tiles[i].id == 0) continue;
        // flat coordinates to 2D coordinates
        spawnablePositions.emplace_back(i % width, i / width);
    }
}
void Area::Tick(Player* player)
{
    SpawnCreature(); // we'll be spawning creatures as long as there's space for them and haven't reached the max count
    // Then, we will mark the positions of the monsters as blocked in the collider
    std::vector<pdcpp::Point<int>> blockPositions = std::vector<pdcpp::Point<int>>();
    for (const auto& monster : livingMonsters)
    {
        blockPositions.emplace_back(monster->GetTiledPosition());
    }
    for (auto blockedPosition : blockPositions)
    {
        collider->block(
            static_cast<float>(blockedPosition.x),
            static_cast<float>(blockedPosition.y),
            true);
    }
    // Then, we will tick the monsters. So they can calculate paths and move
    for (const auto& monster : livingMonsters)
    {
        // We avoid the monsters from blocking itself by unblocking its position before ticking it.
        collider->unblock(
            static_cast<float>(monster->GetTiledPosition().x),
            static_cast<float>(monster->GetTiledPosition().y));
        monster->Tick(player, this);
        collider->block(
            static_cast<float>(monster->GetTiledPosition().x),
            static_cast<float>(monster->GetTiledPosition().y), true);
    }
    // Finally, we will unblock the positions of the monsters
    for (auto blockedPosition : blockPositions)
    {
        collider->unblock(
            static_cast<float>(blockedPosition.x),
            static_cast<float>(blockedPosition.y));
    }
}
Map_Layer Area::ToMapLayer() const
{
    Map_Layer layer(width, std::vector<unsigned short>(height, 0));
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            const Tile& tile = mapData[0].tiles[y * width + x];
            layer[x][y] = tile.collision ? MapCollision::BLOCKS_ALL : MapCollision::BLOCKS_NONE;
        }
    }
    return layer;
}
void Area::SetUpPathfindingContainer()
{
    pathfindingContainer = std::make_shared<AStarContainer>(width, height, width*height);
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (CheckCollision(i * tileWidth, j * tileHeight)) continue;
            auto* node = new AStarNode(pdcpp::Point<int>(i, j));
            pathfindingContainer->Add(node);
        }
    }
}