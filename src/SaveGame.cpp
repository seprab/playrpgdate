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

    // Build JSON document
    std::ostringstream json;
    json << "{\n";
    json << "  \"version\": 1,\n";
    json << "  \"player\": " << SerializePlayer(player) << ",\n";
    json << "  \"area\": {\n";
    json << "    \"id\": " << area->GetId() << ",\n";
    json << "    \"monstersSpawned\": " << area->GetMonstersSpawnedCount() << ",\n";
    json << "    \"monsters\": " << SerializeMonsters(area) << ",\n";
    json << "    \"mapData\": " << SerializeMapData(area) << "\n";
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
    json << "    \"level\": " << player->GetLevel() << ",\n";
    json << "    \"skillPoints\": " << player->GetSkillPoints() << ",\n";
    json << "    \"selectedSkill\": " << player->GetSelectedMagic() << ",\n";
    json << "    \"strength\": " << player->GetStrength() << ",\n";
    json << "    \"agility\": " << player->GetAgility() << ",\n";
    json << "    \"constitution\": " << player->GetConstitution() << ",\n";
    json << "    \"evasion\": " << player->GetEvasion() << "\n";
    json << "  }";

    return json.str();
}

std::string SaveGame::SerializeMonsters(const std::shared_ptr<Area>& area)
{
    std::ostringstream json;
     std::vector<std::shared_ptr<Monster>> monsters = area->GetCreatures(); // This method already return living monsters only
     json << "[\n";
     for (size_t i = 0; i < monsters.size(); i++) {
         auto& monster = monsters[i];
         pdcpp::Point<int> pos = monster->GetPosition();
         json << "    { ";
         json << "\"id\": " << monster->GetId() << ", ";
         json << "\"hp\": " << monster->GetHP() << ", ";
         json << "\"position\": { \"x\": " << pos.x << ", \"y\": " << pos.y << " }";
         json << " }";
         if (i < monsters.size() - 1) json << ",";
         json << "\n";
     }
     json << "  ]";

    return json.str();
}

std::string SaveGame::SerializeMapData(const std::shared_ptr<Area>& area)
{
    return area->SerializeMapData();
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

    // Parse monsters data
    if (!DeserializeMonsters(area, buffer.get(), bytesRead)) {
        Log::Error("SaveGame::Load - Failed to deserialize monsters");
        return false;
    }

    // Parse map data (for procedural maps)
    DeserializeMapData(area, buffer.get(), bytesRead);

    Log::Info("Game loaded successfully from %s", filePath);
    return true;
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
    if ((val = findValue("maxHp"))) {
        player->SetMaxHP(atof(val));
    }

    // Monsters Killed
    if ((val = findValue("monstersKilled"))) {
        player->SetMonstersKilled(atoi(val));
    }

    // Game Start Time
    if ((val = findValue("gameStartTime"))) {
        player->SetGameStartTime(atoi(val));
    }

    // XP / Level / Skills
    if ((val = findValue("xp"))) {
        player->SetXP(static_cast<unsigned int>(atoi(val)));
    }
    if ((val = findValue("level"))) {
        player->SetLevel(static_cast<unsigned int>(atoi(val)));
    }
    if ((val = findValue("skillPoints"))) {
        player->SetSkillPoints(static_cast<unsigned int>(atoi(val)));
    }
    if ((val = findValue("selectedSkill"))) {
        player->SetSelectedMagic(static_cast<unsigned int>(atoi(val)));
    }

    // Stats
    if ((val = findValue("strength"))) {
        player->SetStrength(atoi(val));
    }
    if ((val = findValue("agility"))) {
        player->SetAgility(atoi(val));
    }
    if ((val = findValue("constitution"))) {
        player->SetConstitution(atoi(val));
    }
    if ((val = findValue("evasion"))) {
        player->SetEvasion(static_cast<float>(atof(val)));
    }

    Log::Info("Player deserialized successfully");
    return true;
}

bool SaveGame::DeserializeMonsters(const std::shared_ptr<Area>& area, const char* json, size_t length)
{
    jsmn_parser parser;
    jsmn_init(&parser);

    // Count tokens
    int tokenCount = jsmn_parse(&parser, json, length, nullptr, 0);
    if (tokenCount < 0) {
        Log::Error("SaveGame::DeserializeMonsters - Failed to parse JSON");
        return false;
    }

    std::vector<jsmntok_t> tokens(tokenCount);
    jsmn_init(&parser);
    tokenCount = jsmn_parse(&parser, json, length, tokens.data(), tokenCount);
    if (tokenCount < 0) {
        Log::Error("SaveGame::DeserializeMonsters - Failed to parse JSON tokens");
        return false;
    }

    // Find the "monstersSpawned" count in the JSON
    int monstersSpawnedCount = 0;
    for (int i = 0; i < tokenCount - 1; i++) {
        if (tokens[i].type == JSMN_STRING) {
            int len = tokens[i].end - tokens[i].start;
            if (strncmp(json + tokens[i].start, "monstersSpawned", len) == 0 && len == 15) {
                if (tokens[i + 1].type == JSMN_PRIMITIVE) {
                    char buf[32];
                    int valLen = tokens[i + 1].end - tokens[i + 1].start;
                    if (valLen < 32) {
                        strncpy(buf, json + tokens[i + 1].start, valLen);
                        buf[valLen] = '\0';
                        monstersSpawnedCount = atoi(buf);
                    }
                }
                break;
            }
        }
    }

    // Set the spawned count on the area
    area->SetMonstersSpawnedCount(monstersSpawnedCount);

    // Clear existing living monsters and spawn queue before restoring saved ones
    area->ClearLivingMonsters();
    area->ClearToSpawnMonsters();

    // Refill the spawn queue based on the loaded monstersSpawnedCount
    area->SetupMonstersToSpawn();

    // Find the "monsters" array in the JSON
    int monstersArrayIndex = -1;
    for (int i = 0; i < tokenCount - 1; i++) {
        if (tokens[i].type == JSMN_STRING) {
            int len = tokens[i].end - tokens[i].start;
            if (strncmp(json + tokens[i].start, "monsters", len) == 0 && len == 8) {
                monstersArrayIndex = i + 1;
                break;
            }
        }
    }

    if (monstersArrayIndex == -1 || tokens[monstersArrayIndex].type != JSMN_ARRAY) {
        Log::Info("SaveGame::DeserializeMonsters - No monsters array found or not an array");
        return true; // Not an error, just no monsters to restore
    }

    int monsterCount = tokens[monstersArrayIndex].size;
    if (monsterCount == 0) {
        Log::Info("SaveGame::DeserializeMonsters - No monsters to restore");
        return true;
    }

    // Get the bank of monsters (templates) from the area
    std::vector<std::shared_ptr<Monster>> bankOfMonsters = area->GetMonsterBank();

    // Parse each monster in the array
    int currentTokenIndex = monstersArrayIndex + 1;
    for (int m = 0; m < monsterCount; m++) {
        if (currentTokenIndex >= tokenCount || tokens[currentTokenIndex].type != JSMN_OBJECT) {
            Log::Error("SaveGame::DeserializeMonsters - Invalid monster object at index %d", m);
            continue;
        }

        int monsterObjSize = tokens[currentTokenIndex].size;
        int monsterStartIndex = currentTokenIndex;

        // Extract monster data
        unsigned int monsterId = 0;
        float monsterHp = 0.0f;
        int posX = 0, posY = 0;

        // Parse the monster object
        currentTokenIndex++; // Move to first key in object
        for (int field = 0; field < monsterObjSize; field++) {
            if (currentTokenIndex >= tokenCount) break;

            // Get key
            if (tokens[currentTokenIndex].type != JSMN_STRING) {
                currentTokenIndex += 2; // Skip this key-value pair
                continue;
            }

            int keyLen = tokens[currentTokenIndex].end - tokens[currentTokenIndex].start;
            const char* key = json + tokens[currentTokenIndex].start;
            currentTokenIndex++; // Move to value

            if (currentTokenIndex >= tokenCount) break;

            // Parse value based on key
            if (strncmp(key, "id", keyLen) == 0 && keyLen == 2) {
                if (tokens[currentTokenIndex].type == JSMN_PRIMITIVE) {
                    char buf[32];
                    int valLen = tokens[currentTokenIndex].end - tokens[currentTokenIndex].start;
                    if (valLen < 32) {
                        strncpy(buf, json + tokens[currentTokenIndex].start, valLen);
                        buf[valLen] = '\0';
                        monsterId = atoi(buf);
                    }
                }
                currentTokenIndex++;
            }
            else if (strncmp(key, "hp", keyLen) == 0 && keyLen == 2) {
                if (tokens[currentTokenIndex].type == JSMN_PRIMITIVE) {
                    char buf[32];
                    int valLen = tokens[currentTokenIndex].end - tokens[currentTokenIndex].start;
                    if (valLen < 32) {
                        strncpy(buf, json + tokens[currentTokenIndex].start, valLen);
                        buf[valLen] = '\0';
                        monsterHp = atof(buf);
                    }
                }
                currentTokenIndex++;
            }
            else if (strncmp(key, "position", keyLen) == 0 && keyLen == 8) {
                // This is an object with x and y
                if (tokens[currentTokenIndex].type == JSMN_OBJECT) {
                    int posObjSize = tokens[currentTokenIndex].size;
                    currentTokenIndex++; // Move into the position object

                    for (int posField = 0; posField < posObjSize; posField++) {
                        if (currentTokenIndex >= tokenCount) break;

                        if (tokens[currentTokenIndex].type == JSMN_STRING) {
                            int posKeyLen = tokens[currentTokenIndex].end - tokens[currentTokenIndex].start;
                            const char* posKey = json + tokens[currentTokenIndex].start;
                            currentTokenIndex++; // Move to value

                            if (currentTokenIndex >= tokenCount) break;

                            if (tokens[currentTokenIndex].type == JSMN_PRIMITIVE) {
                                char buf[32];
                                int valLen = tokens[currentTokenIndex].end - tokens[currentTokenIndex].start;
                                if (valLen < 32) {
                                    strncpy(buf, json + tokens[currentTokenIndex].start, valLen);
                                    buf[valLen] = '\0';

                                    if (strncmp(posKey, "x", posKeyLen) == 0 && posKeyLen == 1) {
                                        posX = atoi(buf);
                                    } else if (strncmp(posKey, "y", posKeyLen) == 0 && posKeyLen == 1) {
                                        posY = atoi(buf);
                                    }
                                }
                            }
                            currentTokenIndex++;
                        }
                    }
                } else {
                    currentTokenIndex++; // Skip if not an object
                }
            }
            else {
                // Unknown field, skip value
                currentTokenIndex++;
            }
        }

        // Now find the monster template from bankOfMonsters and restore it
        std::shared_ptr<Monster> monsterTemplate = nullptr;
        for (const auto& bankMonster : bankOfMonsters) {
            if (bankMonster->GetId() == monsterId) {
                monsterTemplate = bankMonster;
                break;
            }
        }

        if (!monsterTemplate) {
            Log::Error("SaveGame::DeserializeMonsters - Monster template with id %u not found in bank", monsterId);
            continue;
        }

        // Create a copy of the monster template
        auto restoredMonster = std::make_shared<Monster>(*monsterTemplate);

        // Restore saved state
        restoredMonster->SetHP(monsterHp);
        restoredMonster->LoadBitmap();
        restoredMonster->SetPosition(pdcpp::Point<int>(posX, posY));

        // Add to living monsters
        area->AddLivingMonster(restoredMonster);

        Log::Info("SaveGame::DeserializeMonsters - Restored monster id=%u, hp=%.1f, pos=(%d,%d)",
                  monsterId, monsterHp, posX, posY);
    }

    Log::Info("SaveGame::DeserializeMonsters - Successfully processed %d monsters", monsterCount);
    return true;
}

bool SaveGame::DeserializeMapData(const std::shared_ptr<Area>& area, const char* json, size_t length)
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

    // Find "area" object
    int areaObjIdx = -1;
    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].type == JSMN_STRING) {
            int len = tokens[i].end - tokens[i].start;
            if (strncmp(json + tokens[i].start, "area", len) == 0 && len == 4) {
                if (i + 1 < tokenCount && tokens[i + 1].type == JSMN_OBJECT) {
                    areaObjIdx = i + 1;
                    break;
                }
            }
        }
    }

    if (areaObjIdx == -1) {
        // No area object found, that's okay (might be old save format)
        return false;
    }

    // Find "mapData" within area object
    int areaObjEnd = tokens[areaObjIdx].end;
    int mapDataIdx = -1;
    
    for (int i = areaObjIdx + 1; i < tokenCount && tokens[i].end < areaObjEnd; i++) {
        if (tokens[i].type == JSMN_STRING) {
            int len = tokens[i].end - tokens[i].start;
            if (strncmp(json + tokens[i].start, "mapData", len) == 0 && len == 7) {
                mapDataIdx = i + 1;
                break;
            }
        }
    }

    if (mapDataIdx == -1 || mapDataIdx >= tokenCount) {
        // No mapData found, that's okay (might not be a procedural map or old save)
        return false;
    }

    // Check if it's null
    if (tokens[mapDataIdx].type == JSMN_PRIMITIVE) {
        int len = tokens[mapDataIdx].end - tokens[mapDataIdx].start;
        if (strncmp(json + tokens[mapDataIdx].start, "null", len) == 0) {
            return false; // No map data
        }
    }

    // Get the mapData object bounds
    int mapDataStart = tokens[mapDataIdx].start;
    int mapDataEnd = tokens[mapDataIdx].end;
    
    // Create a null-terminated substring for the map data
    int mapDataLen = mapDataEnd - mapDataStart;
    std::unique_ptr<char[]> mapDataStr = std::make_unique<char[]>(mapDataLen + 1);
    strncpy(mapDataStr.get(), json + mapDataStart, mapDataLen);
    mapDataStr[mapDataLen] = '\0';

    // Deserialize using Area's method
    std::string mapDataJson(mapDataStr.get());
    bool success = area->DeserializeMapData(mapDataJson);
    
    if (success) {
        Log::Info("SaveGame::DeserializeMapData - Successfully loaded map data");
    }
    
    return success;
}

bool SaveGame::GetAreaIdFromSave(const char* filePath, unsigned int& outAreaId)
{
    if (!pdcpp::FileHelpers::fileExists(filePath)) {
        Log::Info("SaveGame::GetAreaIdFromSave - Save file does not exist: %s", filePath);
        return false;
    }

    // Read file
    auto fileHandle = std::make_unique<pdcpp::FileHandle>(filePath, FileOptions::kFileReadData);
    if (!fileHandle) {
        Log::Error("SaveGame::GetAreaIdFromSave - Failed to open file: %s", filePath);
        return false;
    }

    // Read entire file into buffer
    const size_t MAX_SAVE_SIZE = 64 * 1024; // 64KB max save file
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(MAX_SAVE_SIZE);
    int bytesRead = fileHandle->read(buffer.get(), MAX_SAVE_SIZE - 1);

    if (bytesRead <= 0) {
        Log::Error("SaveGame::GetAreaIdFromSave - Failed to read file or file empty");
        return false;
    }

    buffer[bytesRead] = '\0'; // Null terminate

    // Parse JSON
    jsmn_parser parser;
    jsmn_init(&parser);

    // Count tokens
    int tokenCount = jsmn_parse(&parser, buffer.get(), bytesRead, nullptr, 0);
    if (tokenCount < 0) {
        Log::Error("SaveGame::GetAreaIdFromSave - JSON parse error: %d", tokenCount);
        return false;
    }

    std::vector<jsmntok_t> tokens(tokenCount);
    jsmn_init(&parser);
    tokenCount = jsmn_parse(&parser, buffer.get(), bytesRead, tokens.data(), tokenCount);
    if (tokenCount < 0) {
        Log::Error("SaveGame::GetAreaIdFromSave - JSON parse error: %d", tokenCount);
        return false;
    }

    // Find the "area" object and then "id" within it
    bool foundArea = false;
    for (int i = 0; i < tokenCount - 1; i++) {
        if (tokens[i].type == JSMN_STRING) {
            int len = tokens[i].end - tokens[i].start;
            if (strncmp(buffer.get() + tokens[i].start, "area", len) == 0 && len == 4) {
                foundArea = true;
                // Next token should be the area object
                int areaObjIndex = i + 1;
                if (tokens[areaObjIndex].type == JSMN_OBJECT) {
                    int areaObjSize = tokens[areaObjIndex].size;
                    int currentIndex = areaObjIndex + 1;

                    // Look for "id" key within the area object
                    for (int j = 0; j < areaObjSize; j++) {
                        if (currentIndex >= tokenCount) break;

                        if (tokens[currentIndex].type == JSMN_STRING) {
                            int keyLen = tokens[currentIndex].end - tokens[currentIndex].start;
                            const char* key = buffer.get() + tokens[currentIndex].start;

                            if (strncmp(key, "id", keyLen) == 0 && keyLen == 2) {
                                // Next token is the value
                                currentIndex++;
                                if (currentIndex < tokenCount && tokens[currentIndex].type == JSMN_PRIMITIVE) {
                                    char buf[32];
                                    int valLen = tokens[currentIndex].end - tokens[currentIndex].start;
                                    if (valLen < 32) {
                                        strncpy(buf, buffer.get() + tokens[currentIndex].start, valLen);
                                        buf[valLen] = '\0';
                                        outAreaId = static_cast<unsigned int>(atoi(buf));
                                        Log::Info("SaveGame::GetAreaIdFromSave - Found area ID: %u", outAreaId);
                                        return true;
                                    }
                                }
                            }
                        }
                        currentIndex += 2; // Skip key-value pair
                    }
                }
                break;
            }
        }
    }

    if (!foundArea) {
        Log::Error("SaveGame::GetAreaIdFromSave - 'area' object not found in save file");
    } else {
        Log::Error("SaveGame::GetAreaIdFromSave - 'id' field not found in area object");
    }
    return false;
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
