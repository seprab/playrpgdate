# SaveGame Framework Documentation

## Overview

The SaveGame framework provides a scalable, JSON-based save/load system for your Playdate RPG. It's designed to be easily extensible as you add more data to save.

## Features

✅ **JSON Format** - Human-readable, debuggable save files
✅ **Scalable** - Easy to add new fields without breaking old saves
✅ **Error Handling** - Graceful fallback if save files are missing/corrupted
✅ **Type Safe** - Uses existing Player, Area, Monster classes

## Current Save Data

The framework currently saves:

### Player Data
- Position (x, y)
- HP / Max HP
- Monsters Killed
- Survival Time
- Level
- XP
- Strength
- Agility
- Evasion

### Area Data
- Area ID
- Monsters (placeholder - needs Area::GetMonsters() implementation)

## Usage

### Saving

```cpp
// In GameManager or wherever you need to save
if (!SaveGame::Save(player, activeArea, "saves/save.json")) {
    Log::Error("Failed to save game");
}
```

### Loading

```cpp
// Create player and area first
player = std::make_shared<Player>();
activeArea = std::static_pointer_cast<Area>(entityManager->GetEntity(9002));
activeArea->Load();

// Then load save data
if (!SaveGame::Load(player, activeArea, "saves/save.json")) {
    Log::Error("Failed to load save, using defaults");
    player->SetTiledPosition(pdcpp::Point<int>(23, 32));
}
```

## Adding New Save Data

### 1. Add to Player Serialization

Edit `SaveGame.cpp` in `SerializePlayer()`:

```cpp
json << "    \"newField\": " << player->GetNewField() << ",\n";
```

### 2. Add to Player Deserialization

Edit `SaveGame.cpp` in `DeserializePlayer()`:

```cpp
if ((val = findValue("newField"))) {
    player->SetNewField(atoi(val));  // or atof() for floats
}
```

### 3. For Complex Data (Monsters, Inventory, etc.)

Implement the serialization methods:

```cpp
std::string SaveGame::SerializeMonsters(const std::shared_ptr<Area>& area)
{
    std::ostringstream json;
    std::vector<std::shared_ptr<Monster>> monsters = area->GetMonsters();

    json << "[\n";
    for (size_t i = 0; i < monsters.size(); i++) {
        auto& monster = monsters[i];
        if (monster && monster->IsAlive()) {
            pdcpp::Point<int> pos = monster->GetPosition();
            json << "    {\n";
            json << "      \"id\": " << monster->GetID() << ",\n";
            json << "      \"hp\": " << monster->GetHP() << ",\n";
            json << "      \"position\": { \"x\": " << pos.x << ", \"y\": " << pos.y << " }\n";
            json << "    }";
            if (i < monsters.size() - 1) json << ",";
            json << "\n";
        }
    }
    json << "  ]";

    return json.str();
}
```

## Example Save File

```json
{
  "version": 1,
  "player": {
    "position": { "x": 1024, "y": 768 },
    "hp": 85,
    "maxHp": 100,
    "monstersKilled": 12,
    "survivalTime": 145,
    "level": 3,
    "xp": 2500,
    "strength": 15,
    "agility": 12,
    "evasion": 0.25
  },
  "area": {
    "id": 9002,
    "monsters": []
  }
}
```

## TODO / Future Enhancements

- [ ] Implement monster serialization (requires Area::GetMonsters())
- [ ] Add inventory serialization
- [ ] Add equipment/weapon serialization
- [ ] Add visited areas tracking
- [ ] Add quest/dialogue state
- [ ] Add multiple save slots
- [ ] Add save file versioning/migration for backwards compatibility
- [ ] Compress JSON for smaller file size (optional)

## Notes

- Save files are stored in `saves/save.json`
- The framework uses jsmn (already in your project) for JSON parsing
- All serialization is done in-memory before writing to disk
- Maximum save file size is 64KB (configurable in SaveGame.cpp)

## Troubleshooting

**Save file not loading:**
- Check console for error messages
- Verify file exists at `saves/save.json`
- Check file permissions
- Verify JSON is valid (use an online JSON validator)

**Missing Player setters:**
Some fields like level, XP, strength may not have setters in Player class. You'll need to add them:

```cpp
// In Player.h
void SetLevel(unsigned int lvl) { level = lvl; }
void SetXP(unsigned int experience) { xp = experience; }
```
