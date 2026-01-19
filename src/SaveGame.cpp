#include "SaveGame.h"
#include "Player.h"
#include "Area.h"
#include "Monster.h"
#include "Log.h"
#include "pdcpp/core/File.h"
#include "jsmn.h"
#include <sstream>
#include <vector>

bool SaveGame::Save(
    const std::shared_ptr<Player>& player,
    const std::shared_ptr<Area>& area,
    const char* filePath
)
{
    if (!player || !area) {
        Log::Error("SaveGame::Save - Player or Area is null");
        return false;
    }

    try {
        // Build JSON document
        std::ostringstream json;
        json << "{\n";
        json << "  \"version\": 1,\n";
        json << "  \"player\": " << SerializePlayer(player) << ",\n";
        json << "  \"area\": {\n";
        json << "    \"id\": " << area->GetId() << ",\n";
        json << "    \"monsters\": " << SerializeMonsters(area) << "\n";
        json << "  }\n";
        json << "}\n";

        std::string jsonStr = json.str();

        // Write to file
        auto fileHandle = std::make_unique<pdcpp::FileHandle>(filePath, FileOptions::kFileWrite);
        if (!fileHandle) {
            Log::Error("SaveGame::Save - Failed to open file: %s", filePath);
            return false;
        }

        int bytesWritten = fileHandle->write(const_cast<void*>(static_cast<const void*>(jsonStr.c_str())), jsonStr.length());
        if (bytesWritten != static_cast<int>(jsonStr.length())) {
            Log::Error("SaveGame::Save - Failed to write complete data");
            return false;
        }

        Log::Info("Game saved successfully to %s", filePath);
        return true;
    }
    catch (...) {
        Log::Error("SaveGame::Save - Exception occurred");
        return false;
    }
}

std::string SaveGame::SerializePlayer(const std::shared_ptr<Player>& player)
{
    std::ostringstream json;
    pdcpp::Point<int> pos = player->GetPosition();

    json << "{\n";
    json << "    \"position\": { \"x\": " << pos.x << ", \"y\": " << pos.y << " },\n";
    json << "    \"hp\": " << player->GetHP() << ",\n";
    json << "    \"maxHp\": " << player->GetMaxHP() << ",\n";
    json << "    \"monstersKilled\": " << player->GetMonstersKilled() << ",\n";
    json << "    \"gameStartTime\": " << player->GetSurvivalTimeSeconds() << ",\n";
    json << "    \"xp\": " << player->GetXP() << ",\n";
    json << "    \"strength\": " << player->GetStrength() << ",\n";
    json << "    \"agility\": " << player->GetAgility() << ",\n";
    json << "    \"evasion\": " << player->GetEvasion() << "\n";
    json << "  }";

    return json.str();
}

std::string SaveGame::SerializeMonsters(const std::shared_ptr<Area>& area)
{
    // Note: This requires Area to expose its monsters list
    // For now, return empty array - you'll need to add GetMonsters() to Area class
    std::ostringstream json;
    json << "[]";

    // TODO: Implement when Area exposes monster list
    // std::vector<std::shared_ptr<Monster>> monsters = area->GetMonsters();
    // json << "[\n";
    // for (size_t i = 0; i < monsters.size(); i++) {
    //     auto& monster = monsters[i];
    //     if (monster && monster->IsAlive()) {
    //         pdcpp::Point<int> pos = monster->GetPosition();
    //         json << "    { ";
    //         json << "\"id\": " << monster->GetID() << ", ";
    //         json << "\"hp\": " << monster->GetHP() << ", ";
    //         json << "\"position\": { \"x\": " << pos.x << ", \"y\": " << pos.y << " }";
    //         json << " }";
    //         if (i < monsters.size() - 1) json << ",";
    //         json << "\n";
    //     }
    // }
    // json << "  ]";

    return json.str();
}

bool SaveGame::Load(
    const std::shared_ptr<Player>& player,
    const std::shared_ptr<Area>& area,
    const char* filePath
)
{
    if (!player || !area) {
        Log::Error("SaveGame::Load - Player or Area is null");
        return false;
    }

    if (!pdcpp::FileHelpers::fileExists(filePath)) {
        return false;
    }

    try {
        // Read file
        auto fileHandle = std::make_unique<pdcpp::FileHandle>(filePath, FileOptions::kFileReadData);
        if (!fileHandle) {
            Log::Error("SaveGame::Load - Failed to open file: %s", filePath);
            return false;
        }

        // Read entire file into buffer
        const size_t MAX_SAVE_SIZE = 64 * 1024; // 64KB max save file
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(MAX_SAVE_SIZE);
        int bytesRead = fileHandle->read(buffer.get(), MAX_SAVE_SIZE - 1);

        if (bytesRead <= 0) {
            Log::Error("SaveGame::Load - Failed to read file or file empty");
            return false;
        }

        buffer[bytesRead] = '\0'; // Null terminate

        // Parse JSON
        jsmn_parser parser;
        jsmn_init(&parser);

        // First pass: count tokens
        int tokenCount = jsmn_parse(&parser, buffer.get(), bytesRead, nullptr, 0);
        if (tokenCount < 0) {
            Log::Error("SaveGame::Load - JSON parse error: %d", tokenCount);
            return false;
        }

        // Allocate tokens
        std::vector<jsmntok_t> tokens(tokenCount);
        jsmn_init(&parser);
        tokenCount = jsmn_parse(&parser, buffer.get(), bytesRead, tokens.data(), tokenCount);

        if (tokenCount < 0) {
            Log::Error("SaveGame::Load - JSON parse error: %d", tokenCount);
            return false;
        }

        // Parse player data
        if (!DeserializePlayer(player, buffer.get(), bytesRead)) {
            Log::Error("SaveGame::Load - Failed to deserialize player");
            return false;
        }

        // Parse monsters data (if needed)
        // DeserializeMonsters(area, buffer.get(), bytesRead);

        Log::Info("Game loaded successfully from %s", filePath);
        return true;
    }
    catch (...) {
        Log::Error("SaveGame::Load - Exception occurred");
        return false;
    }
}

bool SaveGame::DeserializePlayer(const std::shared_ptr<Player>& player, const char* json, size_t length)
{
    jsmn_parser parser;
    jsmn_init(&parser);

    // Count tokens
    int tokenCount = jsmn_parse(&parser, json, length, nullptr, 0);
    if (tokenCount < 0) return false;

    std::vector<jsmntok_t> tokens(tokenCount);
    jsmn_init(&parser);
    tokenCount = jsmn_parse(&parser, json, length, tokens.data(), tokenCount);
    if (tokenCount < 0) return false;

    // Helper lambda to find key and get value
    auto findValue = [&](const char* key) -> const char* {
        for (int i = 0; i < tokenCount - 1; i++) {
            if (tokens[i].type == JSMN_STRING) {
                int len = tokens[i].end - tokens[i].start;
                if (strncmp(json + tokens[i].start, key, len) == 0 && strlen(key) == (size_t)len) {
                    // Return the value token after the key
                    int valStart = tokens[i + 1].start;
                    int valEnd = tokens[i + 1].end;
                    static char buf[256];
                    int valLen = valEnd - valStart;
                    if (valLen >= 256) valLen = 255;
                    strncpy(buf, json + valStart, valLen);
                    buf[valLen] = '\0';
                    return buf;
                }
            }
        }
        return nullptr;
    };

    // Parse player fields
    const char* val;

    // Position
    if ((val = findValue("x"))) {
        int x = atoi(val);
        if ((val = findValue("y"))) {
            int y = atoi(val);
            player->SetPosition(pdcpp::Point<int>(x, y));
        }
    }

    // HP
    if ((val = findValue("hp"))) {
        player->SetHP(atof(val));
    }

    // Monsters Killed
    if ((val = findValue("monstersKilled"))) {
        player->SetMonstersKilled(atoi(val));
    }

    // Game Start Time
    if ((val = findValue("gameStartTime"))) {
        player->SetGameStartTime(atoi(val));
    }

    // Stats - note: these might not have setters, so this is optional
    // You may need to add setters to Player class

    Log::Info("Player deserialized successfully");
    return true;
}

bool SaveGame::DeserializeMonsters(const std::shared_ptr<Area>& area, const char* json, size_t length)
{
    // TODO: Implement monster deserialization when needed
    return true;
}

std::string SaveGame::EscapeJSON(const std::string& str)
{
    std::ostringstream escaped;
    for (char c : str) {
        switch (c) {
            case '"':  escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:   escaped << c; break;
        }
    }
    return escaped.str();
}
