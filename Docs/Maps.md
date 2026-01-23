# Map System Documentation

This document explains how maps work in CardoBlast, including both procedural generation (currently used) and the Tiled map workflow (legacy/optional).

## Table of Contents
1. [Current System: Procedural Generation](#current-system-procedural-generation)
2. [Legacy System: Tiled Maps](#legacy-system-tiled-maps)
3. [Map Data Format](#map-data-format)
4. [Technical Details](#technical-details)

---

## Current System: Procedural Generation

CardoBlast currently uses **procedural generation** to create unique maps for each playthrough.

### Why Procedural Generation?

**Advantages**:
- ✅ Infinite replayability - every game is different
- ✅ No manual level design required
- ✅ Automatic connectivity validation
- ✅ Adjustable difficulty via obstacle density
- ✅ Fast iteration on gameplay parameters

**Trade-offs**:
- ❌ No hand-crafted encounters or set pieces
- ❌ Less narrative opportunities
- ❌ Requires validation to ensure playability

### Generation Process

Maps are generated **incrementally** across multiple frames to prevent lag:

```
Frame 1-5:    Initialize grid + boundaries
Frame 6-50:   Place simple obstacles (5 per frame)
Frame 51-60:  Place structured obstacles (1 per frame)
Frame 61:     Validate connectivity
Frame 62-70:  Fix connectivity if needed
Frame 71:     Setup spawnable positions
Frame 72:     Complete!
```

### Generation Steps

#### 1. Initialize Grid
Creates an empty grid of walkable tiles:
```cpp
void Area::InitializeGrid(int width, int height) {
    mapData.resize(2);  // 2 layers: tiles + spawn points
    mapData[0].tiles.resize(width * height);

    for (int i = 0; i < width * height; i++) {
        mapData[0].tiles[i] = {
            id: 1,              // Grass tile ID
            collision: false    // Walkable
        };
    }
}
```

#### 2. Add Boundaries
Adds walls around the perimeter:
```cpp
void Area::AddBoundaries() {
    // Top and bottom walls
    for (int x = 0; x < width; x++) {
        SetTile(x, 0, {id: 2, collision: true});
        SetTile(x, height-1, {id: 2, collision: true});
    }

    // Left and right walls
    for (int y = 0; y < height; y++) {
        SetTile(0, y, {id: 2, collision: true});
        SetTile(width-1, y, {id: 2, collision: true});
    }
}
```

#### 3. Place Simple Obstacles
Scatters random obstacles (rocks, trees, etc.):
```cpp
void Area::PlaceSimpleObstacles() {
    int totalToPlace = (width * height) * obstacleDensity;  // 15% of tiles
    int placedPerFrame = 5;  // Spread work across frames

    for (int i = 0; i < placedPerFrame && placed < totalToPlace; i++) {
        int x = random(2, width - 3);
        int y = random(2, height - 3);
        int size = random(1, 3);  // 1x1 to 3x3

        if (CanPlaceObstacle(x, y, size)) {
            PlaceRectangle(x, y, size, size);
            placed++;
        }
    }
}
```

**Obstacle Density**: 15% (configurable in `Globals.h`)
**Placement Rate**: 5 obstacles per frame

#### 4. Place Structured Obstacles
Adds interesting shapes for tactical gameplay:
```cpp
void Area::PlaceStructuredObstacle() {
    Point pos = FindOpenSpace(minSize: 5);
    int shapeType = random(0, 4);

    switch (shapeType) {
        case 0: PlaceLShape(pos);      break;  // ┗ shape (cover)
        case 1: PlaceTShape(pos);      break;  // ┻ shape (corridor)
        case 2: PlaceWall(pos);        break;  // ── wall (division)
        case 3: PlacePlatform(pos);    break;  // ■ platform (obstacle)
        case 4: PlacePillar(pos);      break;  // · pillar (single tile)
    }
}
```

**Shape Examples**:
```
L-Shape:         T-Shape:         Wall:
  ███              ███            ███████
  █                 █
  █

Platform:        Pillar:
  ███              ·
  ███
  ███
```

**Placement Rate**: 1 structured obstacle per frame
**Count**: 3-8 structures per map

#### 5. Validate Connectivity
Ensures all walkable tiles are connected using flood fill:
```cpp
bool Area::ValidateConnectivity() {
    // Start from center of map
    Point start = {width / 2, height / 2};

    // Flood fill to find all reachable tiles
    unordered_set<Point> visited;
    queue<Point> toVisit;
    toVisit.push(start);

    while (!toVisit.empty()) {
        Point current = toVisit.front();
        toVisit.pop();

        if (visited.count(current)) continue;
        visited.insert(current);

        // Check 4 neighbors (up, down, left, right)
        for (Point neighbor : GetNeighbors(current)) {
            if (IsWalkable(neighbor) && !visited.count(neighbor)) {
                toVisit.push(neighbor);
            }
        }
    }

    // All walkable tiles must be reachable
    int walkableTiles = CountWalkableTiles();
    return visited.size() == walkableTiles;
}
```

**Why This Matters**:
- Prevents unreachable areas
- Ensures monsters can always path to player
- Guarantees spawnable positions exist

#### 6. Fix Connectivity (If Needed)
Removes obstacles to connect isolated areas:
```cpp
void Area::FixConnectivity() {
    // Try removing obstacles in center first (common issue)
    RemoveObstaclesInRadius(width/2, height/2, radius: 3);

    if (ValidateConnectivity()) return;  // Fixed!

    // Systematic removal: try each obstacle
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (IsObstacle(x, y)) {
                RemoveObstacle(x, y);

                if (ValidateConnectivity()) {
                    return;  // This obstacle was blocking connectivity
                }

                // Didn't help, restore it
                PlaceObstacle(x, y);
            }
        }
    }
}
```

### Configuration

All generation parameters in `Globals.h`:

```cpp
namespace Globals {
    // Map dimensions
    constexpr int DEFAULT_MAP_WIDTH = 40;
    constexpr int DEFAULT_MAP_HEIGHT = 40;
    constexpr int MAP_TILE_SIZE = 16;  // 16x16 pixels

    // Obstacle generation
    constexpr float OBSTACLE_DENSITY = 0.15f;  // 15% of tiles
    constexpr int MIN_OBSTACLE_SIZE = 1;
    constexpr int MAX_OBSTACLE_SIZE = 3;
    constexpr int MIN_STRUCTURED_OBSTACLES = 3;
    constexpr int MAX_STRUCTURED_OBSTACLES = 8;

    // Connectivity
    constexpr int MAX_CONNECTIVITY_FIX_ATTEMPTS = 100;
    constexpr int CONNECTIVITY_FIX_RADIUS = 3;
}
```

### Progress Tracking

Loading screen shows generation progress:

```cpp
float Area::GetGenerationProgress() {
    switch (currentStep) {
        case InitializeGrid:
            return 0.05f;

        case AddBoundaries:
            return 0.10f;

        case PlaceSimpleObstacles:
            float simpleProgress = (float)placedSimple / targetSimple;
            return 0.10f + (0.30f * simpleProgress);

        case PlaceStructuredObstacles:
            float structuredProgress = (float)placedStructured / targetStructured;
            return 0.40f + (0.30f * structuredProgress);

        case ValidateConnectivity:
            return 0.70f;

        case FixConnectivity:
            float fixProgress = (float)fixAttempts / maxAttempts;
            return 0.80f + (0.20f * fixProgress);

        case Complete:
            return 1.00f;
    }
}
```

### Spawnable Position Caching

After generation, cache all walkable tiles for spawning:

```cpp
void Area::LoadSpawnablePositions() {
    spawnablePositions.clear();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (IsWalkable(x, y)) {
                spawnablePositions.push_back({x, y});
            }
        }
    }

    Log::Info("Found %d spawnable positions", spawnablePositions.size());
}
```

**Why Cache?**:
- Fast random access during monster spawning
- No need to search for walkable tiles every spawn
- Pre-validated positions

---

## Legacy System: Tiled Maps

CardoBlast **previously** supported Tiled-created maps. This workflow is still functional if you want to create hand-crafted levels.

### Tiled Workflow

#### 1. Assets Required
- **Tileset Image**: `Source/images/areas/hometown-table-16-16.png`
  - 16x16 pixel tiles
  - 1-bit (black and white)
  - Multiple tiles in a grid

- **Tiled Tileset**: `others/tileset.tsx`
  - References the tileset image
  - Defines tile properties (collision, etc.)

- **Tiled Map**: `others/00_downtown_tiledmap.tmx`
  - Uses the tileset
  - Multiple layers
  - Exported to JSON

#### 2. Creating a Map in Tiled

**Step 1**: Create New Tileset
```
Tiled → New Tileset
  - Name: hometown
  - Image: hometown-table-16-16.png
  - Tile Width: 16
  - Tile Height: 16
  - Save as: tileset.tsx
```

**Step 2**: Create New Map
```
Tiled → New Map
  - Width: 40 tiles
  - Height: 40 tiles
  - Tile Width: 16
  - Tile Height: 16
  - Tile Layer Format: Base64 (uncompressed)
```

**Step 3**: Add Layers
- **Layer 0**: Collision/Terrain
  - Contains tiles that block movement
  - Use collision tiles (rocks, walls, etc.)

- **Layer 1**: Spawn Points
  - Contains special markers
  - Used for monster spawn locations

**Step 4**: Paint the Map
- Use the tileset to paint layer 0
- Mark spawn points on layer 1
- Ensure connectivity between areas

**Step 5**: Export to JSON
```
Tiled → File → Export As
  - Format: JSON
  - Save as: 00_downtown_tiledmap.json
  - Location: others/
```

#### 3. JSON Format

Exported JSON structure:
```json
{
  "width": 40,
  "height": 40,
  "tilewidth": 16,
  "tileheight": 16,
  "layers": [
    {
      "name": "Terrain",
      "data": [1, 1, 2, 2, ...],  // Tile IDs (flattened 2D array)
      "width": 40,
      "height": 40
    },
    {
      "name": "SpawnPoints",
      "data": [0, 0, 5, 0, ...],  // 0 = empty, 5 = spawn marker
      "width": 40,
      "height": 40
    }
  ],
  "tilesets": [
    {
      "firstgid": 1,
      "source": "tileset.tsx"
    }
  ]
}
```

#### 4. Loading Tiled Maps

To use a Tiled map instead of procedural generation:

```cpp
// In Area.cpp, replace procedural generation with:
void Area::LoadTiledMap(const char* jsonPath) {
    // Load JSON file
    File mapFile(jsonPath);
    char* jsonData = mapFile.read();

    // Parse with JSMN
    jsmn_parser parser;
    jsmntok_t tokens[2048];
    int tokenCount = jsmn_parse(&parser, jsonData, strlen(jsonData), tokens, 2048);

    // Extract map dimensions
    width = ExtractInt(jsonData, tokens, "width");
    height = ExtractInt(jsonData, tokens, "height");
    tileWidth = ExtractInt(jsonData, tokens, "tilewidth");
    tileHeight = ExtractInt(jsonData, tokens, "tileheight");

    // Extract layers
    for (int i = 0; i < tokenCount; i++) {
        if (IsKey(jsonData, tokens[i], "layers")) {
            ParseLayers(jsonData, tokens, i);
            break;
        }
    }

    // Setup collision map
    SetupCollisionMap();

    // Load spawnable positions
    LoadSpawnablePositions();
}
```

---

## Map Data Format

Regardless of generation method, maps use the same internal format:

### Layer Structure
```cpp
struct Tile {
    unsigned int id;        // Tile image ID (1-255)
    bool collision;         // Can entities walk through?
};

struct Layer {
    std::vector<Tile> tiles;  // Flattened 2D array
    int width;
    int height;
};

class Area {
    std::vector<Layer> mapData;  // mapData[0] = terrain, mapData[1] = spawn
};
```

### Accessing Tiles

**Get tile at position (x, y)**:
```cpp
Tile Area::GetTile(int x, int y, int layer = 0) {
    int index = y * width + x;
    return mapData[layer].tiles[index];
}
```

**Set tile at position (x, y)**:
```cpp
void Area::SetTile(int x, int y, Tile tile, int layer = 0) {
    int index = y * width + x;
    mapData[layer].tiles[index] = tile;
}
```

**Example**:
```
Map grid (5x5):
  0  1  2  3  4
0 [·][·][█][·][·]
1 [·][█][█][█][·]
2 [█][█][·][█][█]
3 [·][█][·][·][·]
4 [·][·][·][·][·]

Flattened array:
[·, ·, █, ·, ·, ·, █, █, █, ·, █, █, ·, █, █, ·, █, ·, ·, ·, ·, ·, ·, ·, ·]
 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24

Tile at (2, 1):
  index = (1 * 5) + 2 = 7
  tiles[7] = █ (collision: true)
```

### Tile ID Mapping

Tile IDs correspond to tileset images:

```cpp
constexpr int TILE_GRASS = 1;      // Walkable grass
constexpr int TILE_WALL = 2;       // Solid wall
constexpr int TILE_ROCK = 3;       // Obstacle
constexpr int TILE_TREE = 4;       // Obstacle
constexpr int SPAWN_MARKER = 5;    // Monster spawn (layer 1)
```

---

## Technical Details

### Collision Map Setup

After map generation/loading, create collision map for pathfinding:

```cpp
void Area::SetupCollisionMap() {
    collider = std::make_shared<MapCollision>();
    collider->ResizeMap(width, height);

    // Copy collision data from tiles
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool blocked = GetTile(x, y).collision;
            collider->SetCollision(x, y, blocked);
        }
    }
}
```

### Rendering

Only render visible tiles (field-of-view culling):

```cpp
void Area::Render(int cameraX, int cameraY, int fovX, int fovY) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tileX = x * tileWidth;
            int tileY = y * tileHeight;

            // Check if tile is visible
            bool visibleX = abs(cameraX - tileX) < fovX;
            bool visibleY = abs(cameraY - tileY) < fovY;

            if (visibleX && visibleY) {
                Tile tile = GetTile(x, y);
                DrawTile(tileX, tileY, tile.id);
            }
        }
    }
}
```

**FOV Settings**:
```cpp
int fovX = SCREEN_WIDTH + TILE_SIZE * 4;   // 400 + 64 = 464
int fovY = SCREEN_HEIGHT + TILE_SIZE * 4;  // 240 + 64 = 304
```

### Memory Usage

**Map size calculation**:
```cpp
int tileCount = width * height;
int layerCount = 2;
int bytesPerTile = sizeof(Tile);  // ~8 bytes

int totalMemory = tileCount * layerCount * bytesPerTile;

// Example: 40x40 map
// 1600 tiles * 2 layers * 8 bytes = 25,600 bytes (~25 KB)
```

---

## Switching Between Systems

### To Use Procedural Generation (Current Default)
```cpp
// In GameManager::LoadNewGame()
activeArea->LoadWithUI(ui.get());  // Generates procedurally with progress
```

### To Use Tiled Maps
```cpp
// 1. Create Tiled map and export to JSON
// 2. Place JSON in data/ folder
// 3. In GameManager::LoadNewGame()
activeArea->LoadTiledMap("data/maps/my_map.json");
```

### Hybrid Approach
You could also combine both:
```cpp
// Generate base map procedurally
activeArea->GenerateProceduralMap(width, height);

// Then add hand-placed elements from Tiled
activeArea->OverlayTiledElements("data/overlays/special_rooms.json");
```

---

## Future Improvements

### Procedural Generation
- [ ] Multiple biomes (forest, desert, cave, etc.)
- [ ] Guaranteed safe zones around spawn
- [ ] Special rooms (treasure, boss arenas)
- [ ] Difficulty scaling based on player level
- [ ] Seed-based generation for sharing maps

### Tiled Integration
- [ ] Hybrid system: procedural + hand-crafted rooms
- [ ] Story mode with designed levels
- [ ] Tutorial area
- [ ] Boss arenas

### Optimization
- [ ] Chunk-based loading for larger maps
- [ ] Tile animation support
- [ ] Parallax background layers
- [ ] Dynamic lighting/fog of war

---

**Last Updated**: 2026-01-23
**Maintainer**: Sergio Prada
