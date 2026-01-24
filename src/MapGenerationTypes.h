#ifndef MAP_GENERATION_TYPES_H
#define MAP_GENERATION_TYPES_H

#include "Globals.h"

/**
 * @file MapGenerationTypes.h
 * @brief Shared types for procedural map generation.
 *
 * This file defines the base configuration struct used by both
 * Area (incremental generation) and ProceduralMapGenerator (blocking generation).
 */

/**
 * @struct MapGenerationParams
 * @brief Base parameters for procedural map generation.
 *
 * This struct contains all tunable parameters for the map generation algorithm.
 * Default values are pulled from Globals.h for consistency.
 *
 * Used by:
 * - Area::generationParams (incremental generation)
 * - ProceduralMapGenerator::GenerationParams (blocking generation with callback)
 */
struct MapGenerationParams
{
    /// Map dimensions
    int width = Globals::DEFAULT_MAP_WIDTH;
    int height = Globals::DEFAULT_MAP_HEIGHT;

    /// Obstacle generation
    float obstacleDensity = Globals::DEFAULT_OBSTACLE_DENSITY;
    int minObstacleSize = Globals::DEFAULT_MIN_OBSTACLE_SIZE;
    int maxObstacleSize = Globals::DEFAULT_MAX_OBSTACLE_SIZE;
    int minStructuredObstacles = Globals::DEFAULT_MIN_STRUCTURED_OBSTACLES;
    int maxStructuredObstacles = Globals::DEFAULT_MAX_STRUCTURED_OBSTACLES;

    /// Random seed (0 = use random seed)
    unsigned int seed = 0;
};

#endif // MAP_GENERATION_TYPES_H
