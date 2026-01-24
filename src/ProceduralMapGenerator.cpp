#include "ProceduralMapGenerator.h"
#include "Log.h"
#include "pdcpp/core/Random.h"
#include <queue>
#include <algorithm>
#include <utility>
#include <vector>

void ProceduralMapGenerator::ReportProgress(const GenerationParams& params, float progress) {
    if (params.progressCallback) {
        params.progressCallback(progress);
    }
}

std::vector<Layer> ProceduralMapGenerator::GenerateMap(const GenerationParams& params) {
    std::vector<Layer> result;
    
    // Initialize random number generator
    pdcpp::Random rng;
    if (params.seed != 0) {
        // If seed is provided, we'd need to seed the RNG
        // For now, use default random behavior
    }
    
    // Step 1: Initialize grid (0% - 10%)
    Layer collisionLayer;
    InitializeGrid(collisionLayer, params.width, params.height);
    
    // Step 1.5: Add boundary obstacles around the edges
    AddBoundaryObstacles(collisionLayer, params.width, params.height);
    
    ReportProgress(params, 0.1f);
    
    // Step 2: Place simple obstacles (10% - 40%)
    PlaceSimpleObstacles(collisionLayer, params.width, params.height, params, rng);
    ReportProgress(params, 0.4f);
    
    // Step 3: Place structured obstacles (40% - 70%)
    PlaceStructuredObstacles(collisionLayer, params.width, params.height, params, rng);
    ReportProgress(params, 0.7f);
    
    // Step 4: Connectivity validation (70% - 80%)
    if (!ValidateConnectivity(collisionLayer, params.width, params.height)) {
        FixConnectivity(collisionLayer, params.width, params.height);
    }
    ReportProgress(params, 0.8f);

    // Step 5: Widen corridors for multiple access paths (80% - 90%)
    WidenCorridors(collisionLayer, params.width, params.height);
    ReportProgress(params, 0.9f);

    // Step 6: Clear player spawn area (90% - 100%)
    ClearPlayerSpawnArea(collisionLayer, params.width, params.height);
    ReportProgress(params, 1.0f);
    
    result.push_back(collisionLayer);

    return result;
}

std::vector<Layer> ProceduralMapGenerator::GenerateMap(int width, int height) {
    GenerationParams params;
    params.width = width;
    params.height = height;
    return GenerateMap(params);
}

void ProceduralMapGenerator::InitializeGrid(Layer& layer, int width, int height) {
    layer.tiles.resize(width * height);
    for (int i = 0; i < width * height; i++) {
        // Use tile ID 1 for walkable ground tiles (will render as imageTable[0] after the -1 adjustment)
        // This ensures walkable tiles are visible
        layer.tiles[i] = {1, false}; // ID 1, no collision
    }
}

void ProceduralMapGenerator::AddBoundaryObstacles(Layer& layer, int width, int height) {
    // Fill the outer edges with obstacles to create boundaries
    // Top and bottom rows
    for (int x = 0; x < width; x++) {
        // Top row
        int topIndex = 0 * width + x;
        layer.tiles[topIndex] = {2, true}; // ID 2, collision enabled
        
        // Bottom row
        int bottomIndex = (height - 1) * width + x;
        layer.tiles[bottomIndex] = {2, true}; // ID 2, collision enabled
    }
    
    // Left and right columns (excluding corners already filled)
    for (int y = 1; y < height - 1; y++) {
        // Left column
        int leftIndex = y * width + 0;
        layer.tiles[leftIndex] = {2, true}; // ID 2, collision enabled
        
        // Right column
        int rightIndex = y * width + (width - 1);
        layer.tiles[rightIndex] = {2, true}; // ID 2, collision enabled
    }
}

void ProceduralMapGenerator::PlaceSimpleObstacles(Layer& layer, int width, int height, const GenerationParams& params, pdcpp::Random& rng) {
    int totalTiles = width * height;
    int obstacleCount = static_cast<int>(totalTiles * params.obstacleDensity);
    
    // Keep 2-tile border walkable (but boundaries are already obstacles, so start from 2)
    int minX = 2;
    int maxX = width - 3;
    int minY = 2;
    int maxY = height - 3;
    
    int placed = 0;
    int attempts = 0;
    const int maxAttempts = obstacleCount * 10; // Try up to 10x the desired count
    
    while (placed < obstacleCount && attempts < maxAttempts) {
        attempts++;
        
        // Random position
        int x = minX + (rng.next() % (maxX - minX + 1));
        int y = minY + (rng.next() % (maxY - minY + 1));
        
        // Random size
        int size = params.minObstacleSize + (rng.next() % (params.maxObstacleSize - params.minObstacleSize + 1));
        
        // Check if we can place obstacle here
        if (CanPlaceObstacle(layer, width, height, x, y, size, size)) {
            PlaceObstacle(layer, width, x, y, size, size);
            placed++;
            
            // Report progress incrementally
            if (placed % 10 == 0 && params.progressCallback) {
                float progress = 0.1f + (0.3f * static_cast<float>(placed) / static_cast<float>(obstacleCount));
                ReportProgress(params, progress);
            }
        }
    }
}

void ProceduralMapGenerator::PlaceStructuredObstacles(Layer& layer, int width, int height, const GenerationParams& params, pdcpp::Random& rng) {
    int obstacleCount = params.minStructuredObstacles + (rng.next() % (params.maxStructuredObstacles - params.minStructuredObstacles + 1));

    // Keep 2-tile border walkable (but boundaries are already obstacles, so start from 2)
    int minX = 2;
    int maxX = width - 7; // Leave room for larger shapes (up to 6 tiles wide)
    int minY = 2;
    int maxY = height - 5; // Leave room for larger shapes (up to 4 tiles tall)
    
    int placed = 0;
    int attempts = 0;
    const int maxAttempts = obstacleCount * 20;
    
    while (placed < obstacleCount && attempts < maxAttempts) {
        attempts++;
        
        int x = minX + (rng.next() % (maxX - minX + 1));
        int y = minY + (rng.next() % (maxY - minY + 1));
        
        int shapeType = rng.next() % 5; // 0=L, 1=T, 2=Wall, 3=Platform, 4=Pillar
        
        bool placedShape = false;
        switch (shapeType) {
            case 0: // L-shape (4 wide × 4 tall max)
                if (x < maxX - 2 && y < maxY - 2) {
                    PlaceLShape(layer, width, height, x, y, rng);
                    placedShape = true;
                }
                break;
            case 1: // T-shape (6 wide × 6 tall max)
                if (x < maxX - 4 && y < maxY - 4) {
                    PlaceTShape(layer, width, height, x, y, rng);
                    placedShape = true;
                }
                break;
            case 2: // Wall
                PlaceWall(layer, width, height, x, y, rng);
                placedShape = true;
                break;
            case 3: // Platform
                if (x < maxX - 3 && y < maxY - 3) {
                    int platformSize = 2 + (rng.next() % 3); // 2x2 to 4x4
                    PlacePlatform(layer, width, height, x, y, platformSize, rng);
                    placedShape = true;
                }
                break;
            case 4: // Pillar (1x1)
                if (CanPlaceObstacle(layer, width, height, x, y, 1, 1)) {
                    PlaceObstacle(layer, width, x, y, 1, 1);
                    placedShape = true;
                }
                break;
        }
        
        if (placedShape) {
            placed++;
            
            // Report progress incrementally
            if (params.progressCallback) {
                float progress = 0.4f + (0.3f * static_cast<float>(placed) / static_cast<float>(obstacleCount));
                ReportProgress(params, progress);
            }
        }
    }
}

bool ProceduralMapGenerator::ValidateConnectivity(const Layer& layer, int width, int height) {
    // Find first walkable tile
    int startX = -1, startY = -1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (!layer.tiles[y * width + x].collision) {
                startX = x;
                startY = y;
                break;
            }
        }
        if (startX != -1) break;
    }
    
    if (startX == -1) return false; // No walkable tiles
    
    // Count total walkable tiles
    int totalWalkable = 0;
    for (const auto& tile : layer.tiles) {
        if (!tile.collision) totalWalkable++;
    }
    
    // Count reachable tiles using flood fill
    int reachable = FloodFillCount(layer, width, height, startX, startY);
    
    // Require at least 95% connectivity
    float connectivity = static_cast<float>(reachable) / static_cast<float>(totalWalkable);
    return connectivity >= 0.95f;
}

void ProceduralMapGenerator::FixConnectivity(Layer& layer, int width, int height) {
    // Simple approach: remove obstacles near edges of isolated regions
    // More sophisticated: identify isolated regions and remove blocking obstacles

    // Try removing obstacles near the center to improve connectivity
    int centerX = width / 2;
    int centerY = height / 2;
    int radius = 3;

    for (int y = centerY - radius; y <= centerY + radius; y++) {
        for (int x = centerX - radius; x <= centerX + radius; x++) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                int index = y * width + x;
                if (layer.tiles[index].collision) {
                    // Restore to walkable ground tile
                    layer.tiles[index] = {1, false};
                }
            }
        }
    }

    // Validate again
    if (!ValidateConnectivity(layer, width, height)) {
        // If still not connected, remove more obstacles
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                int index = y * width + x;
                if (layer.tiles[index].collision) {
                    // Check if this obstacle blocks connectivity
                    layer.tiles[index] = {1, false}; // Temporarily make walkable
                    if (ValidateConnectivity(layer, width, height)) {
                        return; // Fixed!
                    }
                    layer.tiles[index] = {2, true}; // Restore if didn't help
                }
            }
        }
    }
}

void ProceduralMapGenerator::WidenCorridors(Layer& layer, int width, int height) {
    // Widen narrow corridors and chokepoints to ensure multiple access paths
    // Scan for walkable tiles with 3+ obstacle neighbors (narrow chokepoints)

    const int dx[] = {0, 1, 0, -1}; // Up, Right, Down, Left
    const int dy[] = {-1, 0, 1, 0};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int index = y * width + x;

            // Only process walkable tiles
            if (layer.tiles[index].collision) continue;

            // Count obstacle neighbors in 4 cardinal directions
            int obstacleCount = 0;
            int obstacleNeighbors[4] = {0, 0, 0, 0};

            for (int dir = 0; dir < 4; dir++) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                int nIndex = ny * width + nx;

                if (layer.tiles[nIndex].collision) {
                    obstacleCount++;
                    obstacleNeighbors[dir] = 1;
                }
            }

            // If 3 or 4 neighbors are obstacles, this is a narrow chokepoint
            // Remove one obstacle to widen the corridor
            if (obstacleCount >= 3) {
                // Try to remove obstacles in priority order:
                // 1. Remove horizontal obstacles if vertical is blocked (or vice versa)
                // 2. Prefer removing obstacles that create diagonal openings

                // Check if we have opposite sides blocked (corridor)
                bool horizontalBlocked = obstacleNeighbors[1] && obstacleNeighbors[3]; // Right & Left
                bool verticalBlocked = obstacleNeighbors[0] && obstacleNeighbors[2];   // Up & Down

                // If it's a corner (both directions blocked), remove one to widen
                if (horizontalBlocked && verticalBlocked) {
                    // Remove right or down obstacle (arbitrary choice)
                    int removeDir = (obstacleNeighbors[1]) ? 1 : 3;
                    int nx = x + dx[removeDir];
                    int ny = y + dy[removeDir];
                    int nIndex = ny * width + nx;
                    layer.tiles[nIndex] = {1, false};
                }
                // If only horizontal blocked, remove one vertical obstacle
                else if (horizontalBlocked) {
                    int removeDir = (obstacleNeighbors[0]) ? 0 : 2;
                    int nx = x + dx[removeDir];
                    int ny = y + dy[removeDir];
                    int nIndex = ny * width + nx;
                    layer.tiles[nIndex] = {1, false};
                }
                // If only vertical blocked, remove one horizontal obstacle
                else if (verticalBlocked) {
                    int removeDir = (obstacleNeighbors[1]) ? 1 : 3;
                    int nx = x + dx[removeDir];
                    int ny = y + dy[removeDir];
                    int nIndex = ny * width + nx;
                    layer.tiles[nIndex] = {1, false};
                }
                // Otherwise just remove the first obstacle we find
                else {
                    for (int dir = 0; dir < 4; dir++) {
                        if (obstacleNeighbors[dir]) {
                            int nx = x + dx[dir];
                            int ny = y + dy[dir];
                            int nIndex = ny * width + nx;
                            layer.tiles[nIndex] = {1, false};
                            break;
                        }
                    }
                }
            }
        }
    }
}

void ProceduralMapGenerator::ClearPlayerSpawnArea(Layer& layer, int width, int height) {
    // Clear area around map center where player spawns (20, 20 in a 40x40 map)
    // Player needs walkable space to move after spawning
    int centerX = width / 2;
    int centerY = height / 2;
    int clearRadius = 3; // Clear 3 tiles in each direction (7x7 area)

    for (int y = centerY - clearRadius; y <= centerY + clearRadius; y++) {
        for (int x = centerX - clearRadius; x <= centerX + clearRadius; x++) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                int index = y * width + x;
                if (layer.tiles[index].collision) {
                    // Restore to walkable ground tile
                    layer.tiles[index] = {1, false};
                }
            }
        }
    }
}

bool ProceduralMapGenerator::CanPlaceObstacle(const Layer& layer, int width, int height, int x, int y, int sizeX, int sizeY) {
    // Check bounds
    if (x < 0 || y < 0 || x + sizeX > width || y + sizeY > height) {
        return false;
    }
    
    // Check if area is clear
    for (int dy = 0; dy < sizeY; dy++) {
        for (int dx = 0; dx < sizeX; dx++) {
            int index = (y + dy) * width + (x + dx);
            if (layer.tiles[index].collision) {
                return false; // Already occupied
            }
        }
    }
    
    return true;
}

void ProceduralMapGenerator::PlaceObstacle(Layer& layer, int width, int x, int y, int sizeX, int sizeY) {
    for (int dy = 0; dy < sizeY; dy++) {
        for (int dx = 0; dx < sizeX; dx++) {
            int index = (y + dy) * width + (x + dx);
            // Use tile ID 2 for obstacles (will render as imageTable[1] after the -1 adjustment)
            layer.tiles[index] = {2, true}; // ID 2, collision enabled
        }
    }
}

void ProceduralMapGenerator::PlaceLShape(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng) {
    // Create an L-shape scaled 2x: 4 tiles horizontal × 2 tiles tall, with 2×2 vertical extension
    int orientation = rng.next() % 4; // 0=up-right, 1=up-left, 2=down-right, 3=down-left

    // Check if we can place it
    bool canPlace = false;
    switch (orientation) {
        case 0: // Up-right L (horizontal right, vertical up)
            canPlace = (x + 4 <= width && y >= 2) &&
                       CanPlaceObstacle(layer, width, height, x, y, 4, 2) &&
                       CanPlaceObstacle(layer, width, height, x, y-2, 2, 2);
            break;
        case 1: // Up-left L (horizontal right, vertical up from right)
            canPlace = (x + 4 <= width && y >= 2) &&
                       CanPlaceObstacle(layer, width, height, x, y, 4, 2) &&
                       CanPlaceObstacle(layer, width, height, x+2, y-2, 2, 2);
            break;
        case 2: // Down-right L (horizontal right, vertical down)
            canPlace = (x + 4 <= width && y + 4 <= height) &&
                       CanPlaceObstacle(layer, width, height, x, y, 4, 2) &&
                       CanPlaceObstacle(layer, width, height, x, y+2, 2, 2);
            break;
        case 3: // Down-left L (horizontal right, vertical down from right)
            canPlace = (x + 4 <= width && y + 4 <= height) &&
                       CanPlaceObstacle(layer, width, height, x, y, 4, 2) &&
                       CanPlaceObstacle(layer, width, height, x+2, y+2, 2, 2);
            break;
    }

    if (!canPlace) return;

    // Place the L-shape
    PlaceObstacle(layer, width, x, y, 4, 2); // Horizontal part (4 wide × 2 tall)
    switch (orientation) {
        case 0: PlaceObstacle(layer, width, x, y-2, 2, 2); break;    // Vertical extension up-left
        case 1: PlaceObstacle(layer, width, x+2, y-2, 2, 2); break;  // Vertical extension up-right
        case 2: PlaceObstacle(layer, width, x, y+2, 2, 2); break;    // Vertical extension down-left
        case 3: PlaceObstacle(layer, width, x+2, y+2, 2, 2); break;  // Vertical extension down-right
    }
}

void ProceduralMapGenerator::PlaceTShape(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng) {
    // Create a T-shape scaled 2x: 6×2 bar with 2×2 stem
    int orientation = rng.next() % 4; // 0=up, 1=down, 2=left, 3=right

    bool canPlace = false;
    switch (orientation) {
        case 0: // T pointing up
            canPlace = (x + 6 <= width && y >= 2) &&
                       CanPlaceObstacle(layer, width, height, x, y, 6, 2) &&
                       CanPlaceObstacle(layer, width, height, x+2, y-2, 2, 2);
            break;
        case 1: // T pointing down
            canPlace = (x + 6 <= width && y + 4 <= height) &&
                       CanPlaceObstacle(layer, width, height, x, y, 6, 2) &&
                       CanPlaceObstacle(layer, width, height, x+2, y+2, 2, 2);
            break;
        case 2: // T pointing left
            canPlace = (x >= 2 && y + 6 <= height) &&
                       CanPlaceObstacle(layer, width, height, x, y, 2, 6) &&
                       CanPlaceObstacle(layer, width, height, x-2, y+2, 2, 2);
            break;
        case 3: // T pointing right
            canPlace = (x + 4 <= width && y + 6 <= height) &&
                       CanPlaceObstacle(layer, width, height, x, y, 2, 6) &&
                       CanPlaceObstacle(layer, width, height, x+2, y+2, 2, 2);
            break;
    }

    if (!canPlace) return;

    // Place the T-shape
    switch (orientation) {
        case 0: // T pointing up
            PlaceObstacle(layer, width, x, y, 6, 2);      // Horizontal bar
            PlaceObstacle(layer, width, x+2, y-2, 2, 2);  // Vertical stem (centered)
            break;
        case 1: // T pointing down
            PlaceObstacle(layer, width, x, y, 6, 2);      // Horizontal bar
            PlaceObstacle(layer, width, x+2, y+2, 2, 2);  // Vertical stem (centered)
            break;
        case 2: // T pointing left
            PlaceObstacle(layer, width, x, y, 2, 6);      // Vertical bar
            PlaceObstacle(layer, width, x-2, y+2, 2, 2);  // Horizontal stem (centered)
            break;
        case 3: // T pointing right
            PlaceObstacle(layer, width, x, y, 2, 6);      // Vertical bar
            PlaceObstacle(layer, width, x+2, y+2, 2, 2);  // Horizontal stem (centered)
            break;
    }
}

void ProceduralMapGenerator::PlaceWall(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng) {
    bool horizontal = (rng.next() % 2) == 0;
    int length = 2 + (rng.next() % 3); // 2-4 tiles
    
    if (horizontal) {
        if (x + length <= width && CanPlaceObstacle(layer, width, height, x, y, length, 1)) {
            PlaceObstacle(layer, width, x, y, length, 1);
        }
    } else {
        if (y + length <= height && CanPlaceObstacle(layer, width, height, x, y, 1, length)) {
            PlaceObstacle(layer, width, x, y, 1, length);
        }
    }
}

void ProceduralMapGenerator::PlacePlatform(Layer& layer, int width, int height, int x, int y, int size, pdcpp::Random& rng) {
    if (CanPlaceObstacle(layer, width, height, x, y, size, size)) {
        PlaceObstacle(layer, width, x, y, size, size);
    }
}

int ProceduralMapGenerator::FloodFillCount(const Layer& layer, int width, int height, int startX, int startY) {
    std::vector<bool> visited(width * height, false);
    std::queue<std::pair<int, int>> queue;
    
    queue.push(std::make_pair(startX, startY));
    visited[startY * width + startX] = true;
    int count = 1;
    
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};
    
    while (!queue.empty()) {
        std::pair<int, int> current = queue.front();
        queue.pop();
        int x = current.first;
        int y = current.second;
        
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int index = ny * width + nx;
                if (!visited[index] && !layer.tiles[index].collision) {
                    visited[index] = true;
                    queue.push(std::make_pair(nx, ny));
                    count++;
                }
            }
        }
    }
    
    return count;
}
