#include <utility>
#include "Area.h"
#include "Door.h"
#include "Entity.h"
#include "Dialogue.h"
#include "Log.h"
#include "Utils.h"
#include "ProceduralMapGenerator.h"
#include "UI.h"
#include "pdcpp/core/File.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "pdcpp/graphics/Graphics.h"
#include "pdcpp/graphics/Colors.h"
#include "EntityManager.h"
#include "Monster.h"
#include "Player.h"
#include "EnemyProjectile.h"
#include "pdcpp/core/Random.h"
#include <algorithm>
#include <memory>
#include <sstream>
#include "jsmn.h"

Area::Area()
: Entity(0)
{
    // Initialize slowdown ability - ready to use immediately
    lastSlowdownActivationTime = 0;
    lastActivityCheckTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
}

Area::~Area() = default; // Destructor defined here so unique_ptr can see full EnemyProjectile definition

Area::Area(unsigned int _id, const char* _name, std::shared_ptr<Dialogue> _dialogue, const std::vector<std::shared_ptr<Monster>>& _monsters)
: Entity(_id), dialogue(std::move(_dialogue))
{
    SetName(_name);
    for (const std::shared_ptr<Monster>& monster : _monsters)
    {
        bankOfMonsters.push_back(monster);
    }
    // Initialize slowdown ability - ready to use immediately
    lastSlowdownActivationTime = 0;
    lastActivityCheckTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();
    Log::Info("Area created with id: %d, name: %s", _id, _name);
}
Area::Area(const Area &other)
        : Entity(other.GetId()), dialogue(other.dialogue), bankOfMonsters(other.bankOfMonsters)
{
    SetName(other.GetName());
}
Area::Area(Area &&other) noexcept
        : Entity(other.GetId()), dialogue(std::move(other.dialogue)), bankOfMonsters(std::move(other.bankOfMonsters))
{
    SetName(other.GetName());
}

std::shared_ptr<void> Area::DecodeJson(char *buffer, jsmntok_t *tokens, const int size, EntityManager* entityManager)
{
    std::vector<Area> decodedAreas;
    for (int i = 0; i < size; i++)
    {
        if(tokens[i].type != JSMN_OBJECT ) continue;

        int decodedId{};
        std::shared_ptr<Dialogue> decodedDialogue;
        std::vector<std::shared_ptr<Door>> decodedDoors{};
        std::vector<std::shared_ptr<Monster>> decodedCreatures;

        const int endOfObject = tokens[i].end; //get the end of the Area object
        decodedId = std::stoi(Utils::ValueDecoder(buffer, tokens, tokens[i-1].start, endOfObject, "id"));

        i++; //move into the first property of the Area object. Otherwise, the while is invalid
        while(tokens[i].end < endOfObject)
        {
            if(tokens[i].type != JSMN_STRING)
            {
                i++;
                continue;
            }
            std::string parseProperty = Utils::Subchar(buffer, tokens[i].start, tokens[i].end);
            if(parseProperty == "dialogue")
            {
                i++; //move into the dialogue token
                decodedDialogue = std::make_shared<Dialogue>(buffer, tokens, i);
            }
            else if(parseProperty == "doors")
            {
                i++; //move into the doors array token
                int numOfDoors = tokens[i].size;
                i++; //either move to the first element of the array or move to the next property
                if (numOfDoors == 0) continue;
                for (int j = 0; j < numOfDoors; j++)
                {
                    std::string door = Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end);
                    int decodedDoor = std::stoi(door);
                    auto originalInstance = entityManager->GetEntity(decodedDoor);
                    if (originalInstance == nullptr)
                    {
                        Log::Error("Door with ID %d not found", decodedDoor);
                        continue;
                    }
                    decodedDoors.push_back(std::static_pointer_cast<Door>(originalInstance));
                }
                i=i+numOfDoors;
            }
            else if(parseProperty == "creatures")
            {
                i++; //move into the creatures array token
                int numOfCreatures = tokens[i].size;
                i++; //either move to the first element of the array or move to the next property
                for (int j = 0; j < numOfCreatures; j++)
                {
                    int creatureId = std::stoi(Utils::Subchar(buffer, tokens[i+j].start, tokens[i+j].end));
                    auto originalInstance = entityManager->GetEntity(creatureId);
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

        decodedAreas.emplace_back(decodedId, "", decodedDialogue, decodedCreatures);
        i = endOfObject-1; //move back to the end of the Area object
    }
    return std::make_shared<std::vector<Area>>(decodedAreas);
}
void Area::LoadLayers(std::string fileName, int limitOfTokens)
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
            std::string bufferValue = Utils::Subchar(charBuffer.get(), t[i].start, t[i].end);
            if (bufferValue == "data")
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
                    int parsedId = std::stoi(bufferValue);
                    Tile tile{.id =  parsedId, .collision = parsedId != 0};
                    layer.tiles.push_back(tile);
                }
                mapData.push_back(layer);
                i+=t[i].size;
            }
        }
    }
    height = std::stoi(Utils::ValueDecoder(charBuffer.get(), t.get(), 0, t[0].end, "height"));
    width = std::stoi(Utils::ValueDecoder(charBuffer.get(), t.get(), 0, t[0].end, "width"));
    Log::Info("Map loaded, %i width and %i height", width, height);
}
void Area::GenerateProceduralMap(int width, int height, UI* ui)
{
    ProceduralMapGenerator generator;
    ProceduralMapGenerator::GenerationParams params;
    params.width = width;
    params.height = height;
    
    // Set up progress callback if UI is provided
    if (ui) {
        params.progressCallback = [ui](float progress) {
            ui->UpdateLoadingProgress(progress);
            // Force UI to update/render during generation
            // This allows the loading screen to be visible while generation happens
            ui->Update();
        };
    }
    
    // Generate the map
    mapData = generator.GenerateMap(params);
    
    // Set dimensions
    this->width = width;
    this->height = height;
    
    // Initialize tile dimensions (16x16 pixels per tile)
    tileWidth = Globals::MAP_TILE_SIZE;
    tileHeight = Globals::MAP_TILE_SIZE;
    
    // Mark as procedural
    isProcedural = true;
    
    Log::Info("Procedural map generated: %dx%d", width, height);
}
void Area::DrawTileFromLayer(int layer, int x, int y)
{
    int drawX = x * tileWidth;
    int drawY = y * tileHeight;
    pdcpp::Rectangle<int> tileRect(drawX, drawY, tileWidth, tileHeight);

    if (mapData[layer].tiles[(y * width) + x].collision) {
        // Obstacle tiles - draw dark rectangles with white border
        pdcpp::Graphics::fillRectangle(tileRect, pdcpp::Colors::black);
        // Draw border for obstacles to make them stand out
        pdcpp::Graphics::drawRectangle(tileRect, pdcpp::Colors::white);
    } else {
        // Walkable tiles - draw light gray ground
        pdcpp::Graphics::fillRectangle(tileRect, pdcpp::Colors::solid50GrayA);
    }
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
    // Draw enemy projectiles after monsters
    DrawEnemyProjectiles();
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
void Area::LoadWithUI(UI* ui)
{
    StartIncrementalMapGeneration(40, 40, ui);
}

void Area::StartIncrementalMapGeneration(int width, int height, UI* ui)
{
    // Initialize generation state
    currentGenerationStep = GenerationStep::InitializeGrid;
    generationParams.width = width;
    generationParams.height = height;
    generationParams.obstacleDensity = 0.15f;
    generationParams.minObstacleSize = 1;
    generationParams.maxObstacleSize = 3;
    generationParams.minStructuredObstacles = 3;
    generationParams.maxStructuredObstacles = 8;
    generationParams.seed = 0;
    
    this->width = width;
    this->height = height;
    tileWidth = Globals::MAP_TILE_SIZE;
    tileHeight = Globals::MAP_TILE_SIZE;
    
    generationUI = ui;
    generationRNG = pdcpp::Random();
    
    // Calculate targets
    int totalTiles = width * height;
    simpleObstaclesTarget = static_cast<int>(totalTiles * generationParams.obstacleDensity);
    structuredObstaclesTarget = generationParams.minStructuredObstacles + 
                                (generationRNG.next() % (generationParams.maxStructuredObstacles - generationParams.minStructuredObstacles + 1));
    
    simpleObstaclesPlaced = 0;
    structuredObstaclesPlaced = 0;
    simpleObstacleAttempts = 0;
    structuredObstacleAttempts = 0;
    
    // Initialize progress
    if (generationUI) {
        generationUI->UpdateLoadingProgress(0.0f);
    }
}

bool Area::ContinueMapGeneration()
{
    if (currentGenerationStep == GenerationStep::None || currentGenerationStep == GenerationStep::Complete) {
        return true; // Already complete or not started
    }
    
    ProceduralMapGenerator generator;
    
    switch (currentGenerationStep) {
        case GenerationStep::InitializeGrid: {
            // Initialize grid - do this in one step
            generationLayer.tiles.resize(generationParams.width * generationParams.height);
            for (int i = 0; i < generationParams.width * generationParams.height; i++) {
                generationLayer.tiles[i] = {1, false}; // ID 1, no collision
            }
            currentGenerationStep = GenerationStep::AddBoundaries;
            if (generationUI) {
                generationUI->UpdateLoadingProgress(0.05f);
            }
            return false; // Not complete yet
        }
        
        case GenerationStep::AddBoundaries: {
            // Add boundary obstacles - do this in one step
            generator.AddBoundaryObstacles(generationLayer, generationParams.width, generationParams.height);
            currentGenerationStep = GenerationStep::PlaceSimpleObstacles;
            if (generationUI) {
                generationUI->UpdateLoadingProgress(0.1f);
            }
            return false; // Not complete yet
        }
        
        case GenerationStep::PlaceSimpleObstacles: {
            // Place a few simple obstacles per tick
            const int obstaclesPerTick = 5; // Place 5 obstacles per tick
            int minX = 2;
            int maxX = generationParams.width - 3;
            int minY = 2;
            int maxY = generationParams.height - 3;
            
            for (int i = 0; i < obstaclesPerTick && simpleObstaclesPlaced < simpleObstaclesTarget && 
                 simpleObstacleAttempts < maxSimpleObstacleAttempts; i++) {
                simpleObstacleAttempts++;
                
                int x = minX + (generationRNG.next() % (maxX - minX + 1));
                int y = minY + (generationRNG.next() % (maxY - minY + 1));
                int size = generationParams.minObstacleSize + 
                           (generationRNG.next() % (generationParams.maxObstacleSize - generationParams.minObstacleSize + 1));
                
                if (generator.CanPlaceObstacle(generationLayer, generationParams.width, generationParams.height, x, y, size, size)) {
                    generator.PlaceObstacle(generationLayer, generationParams.width, x, y, size, size);
                    simpleObstaclesPlaced++;
                }
            }
            
            // Update progress
            if (generationUI) {
                float progress = 0.1f + (0.3f * static_cast<float>(simpleObstaclesPlaced) / static_cast<float>(simpleObstaclesTarget));
                generationUI->UpdateLoadingProgress(progress);
            }
            
            // Check if done with simple obstacles
            if (simpleObstaclesPlaced >= simpleObstaclesTarget || simpleObstacleAttempts >= maxSimpleObstacleAttempts) {
                currentGenerationStep = GenerationStep::PlaceStructuredObstacles;
                if (generationUI) {
                    generationUI->UpdateLoadingProgress(0.4f);
                }
            }
            return false; // Not complete yet
        }
        
        case GenerationStep::PlaceStructuredObstacles: {
            // Place one structured obstacle per tick
            int minX = 2;
            int maxX = generationParams.width - 5;
            int minY = 2;
            int maxY = generationParams.height - 5;
            
            if (structuredObstaclesPlaced < structuredObstaclesTarget && 
                structuredObstacleAttempts < maxStructuredObstacleAttempts) {
                structuredObstacleAttempts++;
                
                int x = minX + (generationRNG.next() % (maxX - minX + 1));
                int y = minY + (generationRNG.next() % (maxY - minY + 1));
                int shapeType = generationRNG.next() % 5;
                
                bool placedShape = false;
                switch (shapeType) {
                    case 0: // L-shape
                        if (x < maxX - 2 && y < maxY - 2) {
                            generator.PlaceLShape(generationLayer, generationParams.width, generationParams.height, x, y, generationRNG);
                            placedShape = true;
                        }
                        break;
                    case 1: // T-shape
                        if (x < maxX - 2 && y < maxY - 2) {
                            generator.PlaceTShape(generationLayer, generationParams.width, generationParams.height, x, y, generationRNG);
                            placedShape = true;
                        }
                        break;
                    case 2: // Wall
                        generator.PlaceWall(generationLayer, generationParams.width, generationParams.height, x, y, generationRNG);
                        placedShape = true;
                        break;
                    case 3: // Platform
                        if (x < maxX - 3 && y < maxY - 3) {
                            int platformSize = 2 + (generationRNG.next() % 3);
                            generator.PlacePlatform(generationLayer, generationParams.width, generationParams.height, x, y, platformSize, generationRNG);
                            placedShape = true;
                        }
                        break;
                    case 4: // Pillar
                        if (generator.CanPlaceObstacle(generationLayer, generationParams.width, generationParams.height, x, y, 1, 1)) {
                            generator.PlaceObstacle(generationLayer, generationParams.width, x, y, 1, 1);
                            placedShape = true;
                        }
                        break;
                }
                
                if (placedShape) {
                    structuredObstaclesPlaced++;
                }
            }
            
            // Update progress
            if (generationUI) {
                float progress = 0.4f + (0.3f * static_cast<float>(structuredObstaclesPlaced) / static_cast<float>(structuredObstaclesTarget));
                generationUI->UpdateLoadingProgress(progress);
            }
            
            // Check if done with structured obstacles
            if (structuredObstaclesPlaced >= structuredObstaclesTarget || structuredObstacleAttempts >= maxStructuredObstacleAttempts) {
                currentGenerationStep = GenerationStep::ValidateConnectivity;
                if (generationUI) {
                    generationUI->UpdateLoadingProgress(0.7f);
                }
            }
            return false; // Not complete yet
        }
        
        case GenerationStep::ValidateConnectivity: {
            // Validate and fix connectivity - do this in one step
            if (!generator.ValidateConnectivity(generationLayer, generationParams.width, generationParams.height)) {
                generator.FixConnectivity(generationLayer, generationParams.width, generationParams.height);
            }
            
            // Finalize map
            mapData.clear();
            mapData.push_back(generationLayer);
            isProcedural = true;
            
            currentGenerationStep = GenerationStep::Complete;
            if (generationUI) {
                generationUI->UpdateLoadingProgress(1.0f);
            }
            
            // Set up collision and spawn points
            collider = std::make_shared<MapCollision>();
            collider->SetMap(ToMapLayer(), width, height);
            LoadSpawnablePositions();
            SetupMonstersToSpawn();
            
            Log::Info("Procedural map generated incrementally: %dx%d", width, height);
            return true; // Complete!
        }
        
        default:
            return true; // Unknown state, consider complete
    }
}
void Area::LoadFromSavedData()
{
    // This is called when map data was loaded from save file
    // We just need to set up collision and spawn points
    collider = std::make_shared<MapCollision>();
    collider->SetMap(ToMapLayer(), width, height);
    LoadSpawnablePositions();
    SetupMonstersToSpawn();
}
void Area::Unload()
{
    mapData.clear();
    imageTable = nullptr;

    // Clean up all monster vectors
    livingMonsters.clear();
    toSpawnMonsters.clear();

    // Clean up enemy projectiles
    enemyProjectiles.clear();

    // Clean up other resources
    spawnablePositions.clear();
    pathfindingContainer.reset();
    collider.reset();
    
    // Reset generation state
    currentGenerationStep = GenerationStep::None;
    generationLayer.tiles.clear();
    generationUI = nullptr;
}
void Area::SetupMonstersToSpawn()
{
    // Calculate how many monsters still need to be spawned
    // This accounts for monsters that were already spawned when loading a saved game
    int monstersToSetup = Globals::MONSTER_TOTAL_TO_SPAWN - monstersSpawnedCount;

    // Ensure we don't spawn negative monsters
    if (monstersToSetup <= 0) {
        return;
    }

    for (int i=0; i< monstersToSetup; i++)
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
    int monstersToSpawn = Globals::MONSTER_TOTAL_TO_SPAWN - monstersSpawnedCount;
    if (monstersToSpawn <= 0 || toSpawnMonsters.empty()) return; // don't spawn more monsters if the max count is reached
    if (livingMonsters.size() >= Globals::MONSTER_MAX_LIVING_COUNT) return; // don't spawn more monsters if the max count is reached

    auto spawnPos = FindSpawnablePosition(0);
    if (spawnPos.x == 0 && spawnPos.y == 0)
    {
        return; // no spawnable position found
    }

    std::shared_ptr<Monster> monster = toSpawnMonsters[0];
    livingMonsters.push_back(monster);
    livingMonsters.back()->LoadBitmap();
    livingMonsters.back()->SetTiledPosition(spawnPos); // set a default position for the monster
    toSpawnMonsters.erase(toSpawnMonsters.begin());
    ticksSinceLastSpawn = 0; // reset the spawn timer
    monstersSpawnedCount++; // track total spawned monsters
}
/// Find a spawnable position in the area, out of the sight of the player and not colliding with any tile.
pdcpp::Point<int> Area::FindSpawnablePosition(int attemptCount)
{
    if (attemptCount >= Globals::MAX_SPAWN_ATTEMPTS)
    {
        // Reaching the maximum spawn attempts is expected in some cases (e.g., no valid positions available).
        // Cancelling the spawn for this tick and returning fallback position. Logging as a warning for visibility.
        Log::Info("Max spawn attempts reached in Area::FindSpawnablePosition. Returning fallback position {0,0}.");
        return {0,0};
    }
    pdcpp::Point<int> playerPosition = entityManager->GetPlayer()->GetTiledPosition();
    unsigned int randomIndex = random.next() % static_cast<unsigned int>(spawnablePositions.size());
    if (playerPosition.distance(spawnablePositions[randomIndex]) < Globals::MONSTER_SPAWN_RADIUS)
    {
        // if the position is too close to the player, find another one
        return FindSpawnablePosition(attemptCount + 1);
    }
    return spawnablePositions[randomIndex];
}
void Area::LoadSpawnablePositions()
{
    spawnablePositions = std::vector<pdcpp::Point<int>>();
    // Ensure we have at least 2 layers (collision and spawn points)
    for (int i = 0; i < mapData[0].tiles.size(); i++)
    {
        if (mapData[0].tiles[i].id == 0) continue;
        // flat coordinates to 2D coordinates
        spawnablePositions.emplace_back(i % width, i / width);
    }
}

std::string Area::SerializeMapData() const
{
    if (mapData.empty()) {
        return "null"; // Not a procedural map or no data
    }
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"width\": " << width << ",\n";
    json << "  \"height\": " << height << ",\n";
    json << "  \"layers\": [\n";

    for (size_t layerIdx = 0; layerIdx < mapData.size(); layerIdx++) {
        const Layer& layer = mapData[layerIdx];
        json << "    {\n";
        json << "      \"tiles\": [\n";

        for (size_t i = 0; i < layer.tiles.size(); i++) {
            const Tile& tile = layer.tiles[i];
            json << (tile.collision ? "1" : "0");
            if (i < layer.tiles.size() - 1) json << ",";
        }
        json << "\n";
        json << "      ]\n";
        json << "    }";
        if (layerIdx < mapData.size() - 1) json << ",";
        json << "\n";
    }

    json << "  ]\n";
    json << "}";
    
    return json.str();
}

bool Area::DeserializeMapData(const std::string& jsonData)
{
    if (jsonData == "null" || jsonData.empty()) {
        return false;
    }
    
    // Parse JSON
    jsmn_parser parser;
    jsmn_init(&parser);
    
    // First pass: count tokens
    int tokenCount = jsmn_parse(&parser, jsonData.c_str(), jsonData.length(), nullptr, 0);
    if (tokenCount < 0) {
        Log::Error("Area::DeserializeMapData - JSON parse error: %d", tokenCount);
        return false;
    }
    
    // Allocate tokens
    std::vector<jsmntok_t> tokens(tokenCount);
    jsmn_init(&parser);
    tokenCount = jsmn_parse(&parser, jsonData.c_str(), jsonData.length(), tokens.data(), tokenCount);
    
    if (tokenCount < 0) {
        Log::Error("Area::DeserializeMapData - JSON parse error: %d", tokenCount);
        return false;
    }
    
    // Find width and height
    int parsedWidth = 0;
    int parsedHeight = 0;
    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].type == JSMN_STRING) {
            std::string key = Utils::Subchar(jsonData.c_str(), tokens[i].start, tokens[i].end);
            if (key == "width" && i + 1 < tokenCount && tokens[i + 1].type == JSMN_PRIMITIVE) {
                std::string value = Utils::Subchar(jsonData.c_str(), tokens[i + 1].start, tokens[i + 1].end);
                parsedWidth = std::stoi(value);
            } else if (key == "height" && i + 1 < tokenCount && tokens[i + 1].type == JSMN_PRIMITIVE) {
                std::string value = Utils::Subchar(jsonData.c_str(), tokens[i + 1].start, tokens[i + 1].end);
                parsedHeight = std::stoi(value);
            }
        }
    }
    
    if (parsedWidth == 0 || parsedHeight == 0) {
        Log::Error("Area::DeserializeMapData - Invalid width or height");
        return false;
    }
    
    // Find layers array
    int layersArrayIdx = -1;
    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].type == JSMN_STRING) {
            std::string key = Utils::Subchar(jsonData.c_str(), tokens[i].start, tokens[i].end);
            if (key == "layers" && i + 1 < tokenCount && tokens[i + 1].type == JSMN_ARRAY) {
                layersArrayIdx = i + 1;
                break;
            }
        }
    }
    
    if (layersArrayIdx == -1) {
        Log::Error("Area::DeserializeMapData - Layers array not found");
        return false;
    }
    
    // Clear existing map data
    mapData.clear();
    
    // Parse layers
    int numLayers = tokens[layersArrayIdx].size;
    int currentIdx = layersArrayIdx + 1;
    
    for (int layerIdx = 0; layerIdx < numLayers && currentIdx < tokenCount; layerIdx++) {
        if (tokens[currentIdx].type != JSMN_OBJECT) {
            // Skip non-object tokens
            currentIdx++;
            continue;
        }
        
        Layer layer;
        int layerObjEnd = tokens[currentIdx].end;
        
        // Find tiles array in this layer
        int tilesArrayIdx = -1;
        
        for (int i = currentIdx + 1; i < tokenCount && tokens[i].end < layerObjEnd; i++) {
            if (tokens[i].type == JSMN_STRING) {
                std::string key = Utils::Subchar(jsonData.c_str(), tokens[i].start, tokens[i].end);
                if (key == "tiles" && i + 1 < tokenCount && tokens[i + 1].type == JSMN_ARRAY) {
                    tilesArrayIdx = i + 1;
                    break;
                }
            }
        }
        
        if (tilesArrayIdx != -1) {
            int numTiles = tokens[tilesArrayIdx].size;
            int tileIdx = tilesArrayIdx + 1;
            
            // Parse tiles array - simplified format: just "1" or "0" strings
            for (int i = 0; i < numTiles && tileIdx < tokenCount; i++) {
                if (tokens[tileIdx].type == JSMN_PRIMITIVE || tokens[tileIdx].type == JSMN_STRING) {
                    std::string value = Utils::Subchar(jsonData.c_str(), tokens[tileIdx].start, tokens[tileIdx].end);
                    bool collision = (value == "1");
                    // Use tile ID 1 for walkable, 2 for obstacles (matching generation)
                    int tileId = collision ? 2 : 1;
                    layer.tiles.push_back({tileId, collision});
                    tileIdx++;
                } else {
                    tileIdx++;
                }
            }
        }
        
        mapData.push_back(layer);
        
        // Move to next layer object - skip all tokens that belong to current layer
        currentIdx++;
        while (currentIdx < tokenCount && tokens[currentIdx].end <= layerObjEnd) {
            currentIdx++;
        }
    }
    
    // Set dimensions
    width = parsedWidth;
    height = parsedHeight;
    
    // Initialize tile dimensions (16x16 pixels per tile)
    tileWidth = Globals::MAP_TILE_SIZE;
    tileHeight = Globals::MAP_TILE_SIZE;
    
    isProcedural = true;
    
    // Verify we loaded the expected number of tiles
    int expectedTiles = width * height;
    if (!mapData.empty() && !mapData[0].tiles.empty() && mapData[0].tiles.size() != static_cast<size_t>(expectedTiles)) {
        Log::Error("Area::DeserializeMapData - Tile count mismatch: expected %d, got %zu", expectedTiles, mapData[0].tiles.size());
        mapData.clear();
        return false;
    }
    
    Log::Info("Area::DeserializeMapData - Loaded map: %dx%d, %zu layers", width, height, mapData.size());
    return true;
}
void Area::Tick(Player* player)
{
    // Update player activity tracking for slowdown ability
    playerIsActive = player->IsPlayerActive();
    unsigned int currentTime = pdcpp::GlobalPlaydateAPI::get()->system->getCurrentTimeMilliseconds();

    if (playerIsActive)
    {
        // Player is active - deactivate slowdown and reset idle timer
        if (slowdownActive)
        {
            slowdownActive = false;
            // Don't reset lastSlowdownActivationTime - keep cooldown running
        }
        playerIdleTime = 0.0f;
        lastActivityCheckTime = currentTime;
    }
    else
    {
        // Player is idle - check if we can activate slowdown
        unsigned int timeSinceLastSlowdown = currentTime - lastSlowdownActivationTime;
        bool cooldownReady = (timeSinceLastSlowdown >= SLOWDOWN_COOLDOWN);

        if (!slowdownActive && cooldownReady)
        {
            // Activate slowdown ability
            slowdownActive = true;
            lastSlowdownActivationTime = currentTime;
            lastActivityCheckTime = currentTime;
            playerIdleTime = 0.0f;
        }

        if (slowdownActive)
        {
            // Calculate idle time in seconds for slowdown progression
            playerIdleTime = static_cast<float>(currentTime - lastActivityCheckTime) / 1000.0f;
        }
    }

    SpawnCreature(); // we'll be spawning creatures as long as there's space for them and haven't reached the max count

    // Then, we will mark the positions of the monsters as blocked in the collider
    std::vector<pdcpp::Point<int>> blockPositions = std::vector<pdcpp::Point<int>>();
    for (size_t i = 0; i < livingMonsters.size(); ++i)
    {
        auto& monster = livingMonsters[i];
        auto monsterPos = monster->GetTiledPosition();
        blockPositions.emplace_back(monsterPos);
        collider->block(
            static_cast<float>(monsterPos.x),
            static_cast<float>(monsterPos.y),
            true);
        monster->SetCanComputePath((i%staggerAmount) == pathfindingTickCounter); // stagger the pathfinding updates
    }
    pathfindingTickCounter = (pathfindingTickCounter + 1) % staggerAmount;

    // Then, we will tick the monsters. So they can calculate paths and move
    for (const auto& monster : livingMonsters)
    {
        // We avoid the monsters from blocking itself by unblocking its position before ticking it.
        auto monsterPos = monster->GetTiledPosition();
        collider->unblock(
            static_cast<float>(monsterPos.x),
            static_cast<float>(monsterPos.y));

        // remove the monster position from the block positions because its position will change
        auto it = std::ranges::find(blockPositions, monsterPos);
        if (it != blockPositions.end())
        {
            blockPositions.erase(it);
        }


        monster->Tick(player, this);
        monsterPos = monster->GetTiledPosition();
        blockPositions.emplace_back(monsterPos);
        collider->block(
            static_cast<float>(monsterPos.x),
            static_cast<float>(monsterPos.y),
            true);
    }

    // Finally, we will unblock the positions of the monsters
    for (auto blockedPosition : blockPositions)
    {
        collider->unblock(
            static_cast<float>(blockedPosition.x),
            static_cast<float>(blockedPosition.y));
    }

    // Count dead monsters and accumulate XP before removing them
    size_t deadMonsters = 0;
    unsigned int xpGained = 0;
    for (const auto& monster : livingMonsters)
    {
        if (!monster->IsAlive())
        {
            deadMonsters++;
            xpGained += monster->GetXP();
        }
    }
    std::erase_if(livingMonsters,
                  [](const std::shared_ptr<Monster>& monster)
                  { return !monster->IsAlive(); });

    // Increment player's kill count and award XP for each dead monster
    for (size_t i = 0; i < deadMonsters; ++i)
    {
        player->IncrementKillCount();
    }
    if (xpGained > 0)
    {
        player->AddXP(xpGained);
    }
    
    // Update enemy projectiles
    UpdateEnemyProjectiles(player);
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

void Area::CreateEnemyProjectile(pdcpp::Point<int> position, float angle, float speed, unsigned int size, float damage)
{
    if (!entityManager)
    {
        return;
    }
    
    auto playerPtr = entityManager->GetPlayer();
    if (!playerPtr)
    {
        return;
    }
    
    auto projectile = std::make_unique<EnemyProjectile>(
        position,
        std::weak_ptr<Player>(playerPtr),
        angle,
        speed,
        size,
        damage
    );
    enemyProjectiles.push_back(std::move(projectile));
}

void Area::AddEnemyProjectile(std::unique_ptr<EnemyProjectile> projectile)
{
    enemyProjectiles.push_back(std::move(projectile));
}

void Area::UpdateEnemyProjectiles(Player* player)
{
    if (!player)
    {
        return;
    }
    
    // Create a shared_ptr to this Area for the Update call
    // We use a custom deleter that does nothing since Area is managed elsewhere
    std::shared_ptr<Area> areaShared(this, [](Area*) {});
    
    // Update all projectiles
    for (auto& projectile : enemyProjectiles)
    {
        if (projectile && projectile->IsAlive())
        {
            projectile->Update(areaShared);
        }
    }
    
    // Remove dead projectiles
    enemyProjectiles.erase(
        std::remove_if(enemyProjectiles.begin(), enemyProjectiles.end(),
            [](const std::unique_ptr<EnemyProjectile>& p) { return !p || !p->IsAlive(); }),
        enemyProjectiles.end()
    );
}

void Area::DrawEnemyProjectiles() const
{
    for (const auto& projectile : enemyProjectiles)
    {
        if (projectile && projectile->IsAlive())
        {
            projectile->Draw();
        }
    }
}