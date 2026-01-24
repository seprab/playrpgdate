#ifndef PROCEDURAL_MAP_GENERATOR_H
#define PROCEDURAL_MAP_GENERATOR_H

#include "Area.h"
#include "MapGenerationTypes.h"
#include <functional>
#include <vector>

/**
 * @class ProceduralMapGenerator
 * @brief Generates procedural maps for CardoBlast.
 *
 * Provides two generation modes:
 * 1. Blocking: GenerateMap() - generates entire map at once with progress callbacks
 * 2. Helper methods: Used by Area for incremental frame-by-frame generation
 */
class ProceduralMapGenerator {
public:
    /// Progress callback: receives progress value from 0.0f to 1.0f
    using ProgressCallback = std::function<void(float)>;

    /**
     * @struct GenerationParams
     * @brief Extended parameters with progress callback support.
     *
     * Inherits all base parameters from MapGenerationParams and adds
     * optional callback for progress reporting during blocking generation.
     */
    struct GenerationParams : MapGenerationParams {
        ProgressCallback progressCallback = nullptr; ///< Optional progress callback
    };
    
    std::vector<Layer> GenerateMap(const GenerationParams& params);
    std::vector<Layer> GenerateMap(int width, int height); // Convenience method

    // Public methods for incremental generation
    void InitializeGrid(Layer& layer, int width, int height);
    void AddBoundaryObstacles(Layer& layer, int width, int height);
    bool CanPlaceObstacle(const Layer& layer, int width, int height, int x, int y, int sizeX, int sizeY);
    void PlaceObstacle(Layer& layer, int width, int x, int y, int sizeX, int sizeY);
    void PlaceLShape(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng);
    void PlaceTShape(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng);
    void PlaceWall(Layer& layer, int width, int height, int x, int y, pdcpp::Random& rng);
    void PlacePlatform(Layer& layer, int width, int height, int x, int y, int size, pdcpp::Random& rng);
    bool ValidateConnectivity(const Layer& layer, int width, int height);
    void FixConnectivity(Layer& layer, int width, int height);
    void WidenCorridors(Layer& layer, int width, int height);
    void ClearPlayerSpawnArea(Layer& layer, int width, int height);
    int FloodFillCount(const Layer& layer, int width, int height, int startX, int startY);

private:
    void ReportProgress(const GenerationParams& params, float progress);
    void PlaceSimpleObstacles(Layer& layer, int width, int height, const GenerationParams& params, pdcpp::Random& rng);
    void PlaceStructuredObstacles(Layer& layer, int width, int height, const GenerationParams& params, pdcpp::Random& rng);
};

#endif // PROCEDURAL_MAP_GENERATOR_H
