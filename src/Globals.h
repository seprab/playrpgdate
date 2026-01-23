//
// Created by Sergio Prada on 19/06/25.
//

#ifndef GLOBALS_H
#define GLOBALS_H

namespace  Globals
{
    // Constants for game settings
    constexpr int MAX_PATH_FIND_FAILURE_COUNT = 2; // Maximum number of pathfinding failures before resetting
    constexpr int MULT_FOR_RESET_PATH_FIND_FAILURE = 15; // Count to reset pathfinding failure count
    constexpr int MAP_TILE_SIZE = 16; // Size of each tile in the map
    constexpr int PLAYER_SIZE = 16; // Size of the player character
    constexpr int PLAYER_FOV_X = 234; // Field of view width for the player
    constexpr int PLAYER_FOV_Y = 136; // Field of view height for the player
    constexpr int MONSTER_AWARENESS_RADIUS = 50; // Radius within which monsters can detect the player
    constexpr int MONSTER_SPAWN_RADIUS = 20; // Radius within which monsters can spawn around the player
    constexpr int MONSTER_ATTACK_RANGE = 2; // Range within which monsters can attack the player (melee)
    constexpr int MONSTER_RANGED_ATTACK_RANGE = 15; // Range at which monsters can shoot projectiles
    constexpr float MONSTER_DIAGONAL_MOVE_SCALE = 0.7f; // Scale factor for diagonal movement speed of monsters
    constexpr int MONSTER_KITE_MIN_RANGE = 3; // Minimum distance for kiting monsters
    constexpr int MONSTER_KITE_MAX_RANGE = 7; // Maximum distance for kiting monsters
    constexpr int MONSTER_KITE_STEP = 4; // Tiles to move away when kiting
    constexpr int MONSTER_MAX_LIVING_COUNT = 10; // Maximum number of monsters that can be present in an area
    constexpr int MONSTER_TOTAL_TO_SPAWN = 50; // Total number of monsters to spawn in an area
    constexpr int TICKS_BETWEEN_MONSTER_SPAWNS = 60; // Number of ticks between monster spawns
    constexpr int MONSTER_RANDOM_SPACING = 4; // Random spacing for monster when moving and finding a blocker
    constexpr int MAX_SPAWN_ATTEMPTS = 10; // Maximum number of attempts to spawn a monster
    constexpr int PATH_FINDING_COOLDOWN = 50; // Cooldown for pathfinding in ticks
    constexpr int MAX_ENTITY_FLASHING_TICKS = 10; // Maximum ticks an entity will flash when damaged
    constexpr float AUTO_FIRE_RANGE = 100.0f;
    constexpr unsigned int XP_BASE = 10;
    constexpr unsigned int XP_FACTOR = 5;
    constexpr const char* GAME_SAVE_PATH = "savegame.data";
    constexpr const char* MAX_SCORE_PATH = "maxscore.data";
    
    // Enemy ranged attack constants
    constexpr unsigned int MONSTER_RANGED_ATTACK_COOLDOWN = 2000; // Cooldown for ranged monsters in milliseconds
    constexpr unsigned int MONSTER_STATIONARY_ATTACK_COOLDOWN = 4000; // Cooldown for stationary monsters in milliseconds
    constexpr int MONSTER_STATIONARY_PROJECTILE_COUNT = 5; // Number of projectiles for spread shot
    constexpr float MONSTER_STATIONARY_SPREAD_ANGLE = 0.8f; // Angle spread in radians (~45 degrees)
    constexpr float MONSTER_PROJECTILE_SPEED = 6.0f; // Speed of enemy projectiles
    constexpr unsigned int MONSTER_PROJECTILE_SIZE = 6; // Size of enemy projectiles
    constexpr float MONSTER_PROJECTILE_BASE_DAMAGE = 0.5f; // Base damage of enemy projectiles (before strength scaling)
    constexpr float MONSTER_PROJECTILE_STRENGTH_MULTIPLIER = 0.2f; // Damage multiplier per strength point
    constexpr float MONSTER_STATIONARY_PROJECTILE_BASE_DAMAGE = 1.0f; // Base damage for stationary monster projectiles (more powerful)
    constexpr float MONSTER_STATIONARY_PROJECTILE_STRENGTH_MULTIPLIER = 0.3f; // Damage multiplier per strength point for stationary monsters
    constexpr float MONSTER_PROJECTILE_MIN_DAMAGE = 0.5f; // Minimum damage floor to ensure projectiles always do some damage
}
#endif //GLOBALS_H
