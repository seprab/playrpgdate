#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <memory>
#include <string>
#include "pd_api.h"
#include "pdcpp/graphics/Point.h"

class Player;
class Area;
class Monster;

/**
 * SaveGame - A scalable JSON-based save/load framework
 *
 * Usage:
 *   SaveGame::Save(player, area, "saves/save.json");
 *   SaveGame::Load(player, area, entityManager, "saves/save.json");
 */
class SaveGame
{
public:
    /**
     * Save game state to JSON file
     * @param player The player to save
     * @param area The current area
     * @param filePath Path to save file (e.g., "saves/save.json")
     * @return true if successful, false otherwise
     */
    static bool Save(
        const std::shared_ptr<Player>& player,
        const std::shared_ptr<Area>& area,
        const char* filePath
    );

    /**
     * Load game state from JSON file
     * @param player Player instance to load data into (must be pre-created)
     * @param area Area instance to load data into (must be pre-created and loaded)
     * @param filePath Path to save file (e.g., "saves/save.json")
     * @return true if successful, false otherwise
     */
    static bool Load(
        const std::shared_ptr<Player>& player,
        const std::shared_ptr<Area>& area,
        const char* filePath
    );

private:
    // Helper to build JSON string for player
    static std::string SerializePlayer(const std::shared_ptr<Player>& player);

    // Helper to build JSON string for monsters in area
    static std::string SerializeMonsters(const std::shared_ptr<Area>& area);

    // Helper to parse player data from JSON
    static bool DeserializePlayer(const std::shared_ptr<Player>& player, const char* json, size_t length);

    // Helper to parse monsters data from JSON
    static bool DeserializeMonsters(const std::shared_ptr<Area>& area, const char* json, size_t length);

    // JSON string escaping helper
    static std::string EscapeJSON(const std::string& str);
};

#endif // SAVEGAME_H
