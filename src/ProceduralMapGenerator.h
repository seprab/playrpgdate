#ifndef PROCEDURAL_MAP_GENERATOR_H
#define PROCEDURAL_MAP_GENERATOR_H

#include "Area.h"
#include <functional>
#include <vector>

class ProceduralMapGenerator {
public:
    // Progress callback: receives progress value from 0.0f to 1.0f
    using ProgressCallback = std::function<void(float)>;
    
    struct GenerationParams {
        int width = 40;
        int height = 40;
        float obstacleDensity = 0.15f; // 15% of tiles are obstacles
        int minObstacleSize = 1;
        int maxObstacleSize = 3;
        int minStructuredObstacles = 3;
        int maxStructuredObstacles = 8;
        unsigned int seed = 0; // 0 = random seed
        ProgressCallback progressCallback = nullptr; // Optional progress callback
    };
    
    std::vector<Layer> GenerateMap(const GenerationParams& params);
    std::vector<Layer> GenerateMap(int width, int height); // Convenience method

private:
    void ReportProgress(const GenerationParams& params, float progress);
    void InitializeGrid(Layer& layer, int width, int height);
    void AddBoundaryObstacles(Layer& layer, int width, int height);
    void PlaceSimpleObstacles(Layer& layer, int width, int height, const GenerationParams& params, pdcpp::Random& rng);
    void PlaceStructuredObstacles(Layer& layer, int width, int height, const GenerationParams& params, pdcpp::Random& rng);
    bool ValidateConnectivity(const Layer& layer, int width, int height);
    void FixConnectivity(Layer& layer, int width, int height);

    bool CanPlaceObstacle(const Layer& layer, int width, int height, int x, int y, int sizeX, int sizeY);
    void PlaceObstacle(Layer& layer, int width, int x, int y, int sizeX, int sizeY);
    void PlaceLShape(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng);
    void PlaceTShape(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng);
    void PlaceWall(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng);
    void PlacePlatform(Layer& layer, int width, int height, int x, int y, int size, pdcpp::Random& rng);
    
    int FloodFillCount(const Layer& layer, int width, int height, int startX, int startY);
};

#endif // PROCEDURAL_MAP_GENERATOR_H
