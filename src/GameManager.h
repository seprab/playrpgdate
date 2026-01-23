#ifndef MY_PLAYDATE_TEST_GAME_MANAGER_H
#define MY_PLAYDATE_TEST_GAME_MANAGER_H

/**
 * @file GameManager.h
 * @brief Main game controller that owns and coordinates all game systems.
 *
 * GameManager is the top-level class that:
 * - Owns EntityManager, Player, Area, and UI
 * - Runs the main game loop (Update)
 * - Handles save/load operations
 * - Manages game state (loading, playing, game over)
 * - Controls camera positioning
 *
 * Ownership hierarchy:
 *   GameManager
 *   ├── EntityManager (unique_ptr)
 *   ├── Player (shared_ptr)
 *   ├── Area (shared_ptr)
 *   └── UI (shared_ptr)
 */

#include "pdcpp/pdnewlib.h"
#include "EntityManager.h"
#include "Player.h"
#include "UI.h"
#include "SaveGame.h"
#include "pdcpp/graphics/Point.h"

/**
 * @class GameManager
 * @brief Central controller for all game systems and state.
 *
 * This class orchestrates the entire game, owning all major systems and
 * managing the game loop. It handles initialization, updates, rendering,
 * and cleanup.
 */
class GameManager {

public:
    /**
     * @brief Construct GameManager and initialize all systems.
     * @param api Playdate SDK API pointer
     *
     * Initializes EntityManager, UI, sets up callbacks, and loads max score.
     */
    explicit GameManager(PlaydateAPI* api);

    ~GameManager();

    /**
     * @brief Main game loop - called every frame (20 FPS).
     *
     * Responsibilities:
     * - Update player and area (if game is running)
     * - Continue map generation (if loading)
     * - Update camera position
     * - Render all game objects
     * - Handle level-up popup
     * - Check for game over
     */
    void Update();

    /**
     * @brief Start a new game with procedural map generation.
     *
     * Loads entities from JSON, creates new player, generates procedural map,
     * and switches to gameplay.
     */
    void LoadNewGame();

    /**
     * @brief Resume from saved game state.
     *
     * Loads player and area state from saves/save.json.
     * Falls back to default position if save file missing or corrupted.
     */
    void LoadSavedGame();

    /**
     * @brief Persist current game state to disk.
     *
     * Serializes player stats, position, and area state to saves/save.json.
     */
    void SaveGame();

    /**
     * @brief Reset game state and return to main menu.
     *
     * Cleans up player and area, updates max score if needed.
     */
    void CleanGame();

    /**
     * @brief Pause the game (currently unused).
     */
    void PauseGame() const;

    /**
     * @brief Save the highest survival time to disk.
     */
    void SaveMaxScore();

    /**
     * @brief Get the highest survival time achieved.
     * @return Max score in seconds
     */
    [[nodiscard]] int GetMaxScore() const { return maxScore; }

private:
    PlaydateAPI* pd;                                   ///< Playdate SDK API pointer
    std::unique_ptr<EntityManager> entityManager;      ///< Entity registry and loader
    std::shared_ptr<Player> player;                    ///< Player character
    std::shared_ptr<UI> ui;                           ///< User interface system
    std::shared_ptr<Area> activeArea;                 ///< Current level/map
    pdcpp::Point<int> currentCameraOffset = {0,0};     ///< Camera position for smooth follow
    bool isGameRunning = false;                        ///< True when gameplay is active
    int maxScore = 0;                                  ///< Highest survival time (seconds)

    /**
     * @brief Load highest score from maxScore.txt.
     */
    void LoadMaxScore();
};


#endif //MY_PLAYDATE_TEST_GAME_MANAGER_H
