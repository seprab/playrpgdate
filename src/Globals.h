//
// Created by Sergio Prada on 19/06/25.
//

#ifndef GLOBALS_H
#define GLOBALS_H

/**
 * @file Globals.h
 * @brief Central configuration file for all game constants.
 *
 * This file contains all tunable game parameters organized by category.
 * All values are constexpr for compile-time optimization and type safety.
 *
 * To modify game balance, adjust values here and recompile.
 * No magic numbers should exist outside this file.
 *
 * Categories:
 * - Pathfinding
 * - Map & Rendering
 * - Player Stats
 * - Monster Behavior
 * - Combat & Damage
 * - Progression
 * - Visuals
 * - Performance
 */

namespace Globals
{
    // ========================================================================
    // PATHFINDING CONSTANTS
    // ========================================================================
    constexpr int MAX_PATH_FIND_FAILURE_COUNT = 2;      ///< Max pathfinding failures before reset
    constexpr int MULT_FOR_RESET_PATH_FIND_FAILURE = 15; ///< Count to reset pathfinding failure
    constexpr int PATH_FINDING_COOLDOWN = 50;           ///< Cooldown between path recalculations (ticks)

    // ========================================================================
    // MAP & RENDERING CONSTANTS
    // ========================================================================
    constexpr int MAP_TILE_SIZE = 16;                    ///< Size of each tile in pixels (16x16)
    constexpr int PLAYER_SIZE = 16;                      ///< Player sprite size (16x16)
    constexpr int PLAYER_FOV_X = 234;                   ///< Render distance X (screen width + buffer)
    constexpr int PLAYER_FOV_Y = 136;                   ///< Render distance Y (screen height + buffer)
    constexpr int DEFAULT_MAP_WIDTH = 40;               ///< Procedural map width (tiles)
    constexpr int DEFAULT_MAP_HEIGHT = 40;              ///< Procedural map height (tiles)

    // ========================================================================
    // MONSTER BEHAVIOR CONSTANTS
    // ========================================================================
    constexpr int MONSTER_AWARENESS_RADIUS = 50;        ///< Monster detection range (pixels)
    constexpr int MONSTER_SPAWN_RADIUS = 20;            ///< Min distance from player to spawn (tiles)
    constexpr int MONSTER_ATTACK_RANGE = 2;             ///< Melee attack range (tiles)
    constexpr int MONSTER_RANGED_ATTACK_RANGE = 15;    ///< Ranged attack range (tiles)
    constexpr float MONSTER_DIAGONAL_MOVE_SCALE = 0.7f; ///< Diagonal movement speed multiplier
    constexpr int MONSTER_KITE_MIN_RANGE = 3;          ///< Kiting: minimum distance (tiles)
    constexpr int MONSTER_KITE_MAX_RANGE = 7;          ///< Kiting: maximum distance (tiles)
    constexpr int MONSTER_KITE_STEP = 20;               ///< Kiting: retreat distance (tiles)
    constexpr int MONSTER_RANDOM_SPACING = 4;          ///< Random offset when blocked (tiles)
    constexpr int MAX_SPAWN_ATTEMPTS = 10;             ///< Max attempts to find spawn position

    // ========================================================================
    // SPAWNING CONSTANTS
    // ========================================================================
    constexpr int MONSTER_MAX_LIVING_COUNT = 10;       ///< Max simultaneous living monsters
    constexpr int MONSTER_TOTAL_TO_SPAWN = 50;         ///< Total monsters per game
    constexpr int TICKS_BETWEEN_MONSTER_SPAWNS = 60;   ///< Spawn interval (3 seconds @ 20 FPS)

    // ========================================================================
    // COMBAT & DAMAGE CONSTANTS
    // ========================================================================
    constexpr int MAX_ENTITY_FLASHING_TICKS = 10;      ///< Damage flash duration (ticks)
    constexpr float AUTO_FIRE_RANGE = 100.0f;          ///< Auto-fire targeting range (pixels)

    // Enemy ranged attack constants
    constexpr unsigned int MONSTER_RANGED_ATTACK_COOLDOWN = 2000;  ///< Ranged cooldown (ms)
    constexpr unsigned int MONSTER_STATIONARY_ATTACK_COOLDOWN = 4000; ///< Stationary cooldown (ms)
    constexpr int MONSTER_STATIONARY_PROJECTILE_COUNT = 5;     ///< Spread shot count
    constexpr float MONSTER_STATIONARY_SPREAD_ANGLE = 0.8f;    ///< Spread angle (~45°)
    constexpr float MONSTER_PROJECTILE_SPEED = 6.0f;           ///< Projectile speed
    constexpr unsigned int MONSTER_PROJECTILE_SIZE = 6;        ///< Projectile size (pixels)
    constexpr float MONSTER_PROJECTILE_BASE_DAMAGE = 0.5f;     ///< Base damage (ranged)
    constexpr float MONSTER_PROJECTILE_STRENGTH_MULTIPLIER = 0.2f; ///< Damage per strength
    constexpr float MONSTER_STATIONARY_PROJECTILE_BASE_DAMAGE = 1.0f; ///< Base damage (stationary)
    constexpr float MONSTER_STATIONARY_PROJECTILE_STRENGTH_MULTIPLIER = 0.3f; ///< Damage per strength
    constexpr float MONSTER_PROJECTILE_MIN_DAMAGE = 0.5f;      ///< Minimum damage floor

    // ========================================================================
    // PLAYER CONSTANTS
    // ========================================================================
    constexpr int PLAYER_DEFAULT_HP = 100;               ///< Starting HP
    constexpr int PLAYER_DEFAULT_STRENGTH = 10;         ///< Starting strength
    constexpr int PLAYER_DEFAULT_AGILITY = 5;           ///< Starting agility
    constexpr int PLAYER_DEFAULT_CONSTITUTION = 5;      ///< Starting constitution
    constexpr float PLAYER_DEFAULT_EVASION = 0.1f;      ///< Starting evasion (10%)
    constexpr unsigned int PLAYER_AUTO_FIRE_COOLDOWN = 1000; ///< Auto-fire cooldown (ms)
    constexpr unsigned int PLAYER_ACTIVITY_THRESHOLD = 500;  ///< Idle threshold for slowdown (ms)
    constexpr float PLAYER_MOVEMENT_SPEED = 5.0f;       ///< Movement speed (pixels/frame)

    // ========================================================================
    // PROGRESSION CONSTANTS
    // ========================================================================
    constexpr unsigned int XP_BASE = 10;                 ///< Base XP for level 0→1
    constexpr unsigned int XP_FACTOR = 5;                ///< XP scaling factor per level

    // ========================================================================
    // VISUAL CONSTANTS
    // ========================================================================
    constexpr int HEALTH_BAR_WIDTH = 25;                 ///< Health bar width (pixels)
    constexpr int HEALTH_BAR_HEIGHT = 4;                 ///< Health bar height (pixels)
    constexpr int HEALTH_BAR_OFFSET_X = -5;              ///< Health bar X offset
    constexpr int HEALTH_BAR_OFFSET_Y = -10;             ///< Health bar Y offset

    // ========================================================================
    // PERFORMANCE CONSTANTS
    // ========================================================================
    constexpr int GAME_REFRESH_RATE = 20;                ///< Target FPS (20 Hz)

    // ========================================================================
    // FILE PATHS
    // ========================================================================
    constexpr const char* GAME_SAVE_PATH = "savegame.data";    ///< Save file path
    constexpr const char* MAX_SCORE_PATH = "maxscore.data";    ///< Max score file path
}
#endif //GLOBALS_H
