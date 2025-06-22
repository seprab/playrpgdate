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
    constexpr int MONSTER_AWARENESS_RADIUS = 15; // Radius within which monsters can detect the player
    constexpr int MONSTER_ATTACK_RANGE = 1; // Range within which monsters can attack the player
    constexpr float MONSTER_DIAGONAL_MOVE_SCALE = 0.7f; // Scale factor for diagonal movement speed of monsters
    // Other global constants can be added here as needed
}
#endif //GLOBALS_H
