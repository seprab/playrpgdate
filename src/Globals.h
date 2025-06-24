//
// Created by Sergio Prada on 19/06/25.
//

#ifndef GLOBALS_H
#define GLOBALS_H

namespace  Globals
{
    // Constants for game settings
    constexpr int MAX_PATH_FIND_FAILURE_COUNT = 2; // Maximum number of pathfinding failures before resetting
    constexpr int MULT_FOR_RESET_PATH_FIND_FAILURE = 10; // Count to reset pathfinding failure count
    constexpr int MAP_TILE_SIZE = 16; // Size of each tile in the map
    constexpr int PLAYER_SIZE = 16; // Size of the player character
    constexpr int PLAYER_FOV_X = 234; // Field of view width for the player
    constexpr int PLAYER_FOV_Y = 136; // Field of view height for the player
    constexpr int MONSTER_AWARENESS_RADIUS = 50; // Radius within which monsters can detect the player
    constexpr int MONSTER_SPAWN_RADIUS = 20; // Radius within which monsters can spawn around the player
    constexpr int MONSTER_ATTACK_RANGE = 1; // Range within which monsters can attack the player
    constexpr float MONSTER_DIAGONAL_MOVE_SCALE = 0.7f; // Scale factor for diagonal movement speed of monsters
    constexpr int MONSTER_MAX_LIVING_COUNT = 10; // Maximum number of monsters that can be present in an area
    constexpr int MONSTER_TOTAL_TO_SPAWN = 50; // Total number of monsters to spawn in an area
    constexpr int TICKS_BETWEEN_MONSTER_SPAWNS = 60; // Number of ticks between monster spawns
}
#endif //GLOBALS_H
