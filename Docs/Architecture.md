# CardoBlast Architecture Documentation

This document outlines the architectural decisions, design patterns, and system organization of CardoBlast.

## Table of Contents
1. [High-Level Architecture](#high-level-architecture)
2. [Class Hierarchy](#class-hierarchy)
3. [Memory Management](#memory-management)
4. [Core Systems](#core-systems)
5. [Game Loop](#game-loop)
6. [Data Flow](#data-flow)
7. [Design Patterns](#design-patterns)
8. [Performance Considerations](#performance-considerations)

---

## High-Level Architecture

CardoBlast follows a component-based architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────┐
│                   main.cpp                      │
│         (Entry point, event handling)           │
└────────────────┬────────────────────────────────┘
                 │
                 ├─> GameManager (owns everything)
                 │   ├─> EntityManager (entity registry)
                 │   ├─> Player (player state & input)
                 │   ├─> Area (current level/map)
                 │   │   ├─> Monsters (enemies)
                 │   │   ├─> MapCollision (pathfinding)
                 │   │   └─> EnemyProjectiles
                 │   └─> UI (screens & rendering)
                 │
                 └─> Playdate SDK (graphics, input, system)
```

### Key Principles
1. **Single Responsibility** - Each class has one clear purpose
2. **Dependency Injection** - Dependencies passed via constructor
3. **Resource Ownership** - Clear ownership using smart pointers
4. **Game Loop Separation** - Update and render logic separated
5. **Data-Driven** - Game content loaded from JSON files

---

## Class Hierarchy

### Entity Hierarchy

```
Entity (abstract base)
├── Creature (adds stats, combat)
│   ├── Player (player character)
│   └── Monster (enemy AI)
├── Area (levels/maps)
├── Item
│   ├── Weapon
│   └── Armor
└── Door

Magic (spell system)
├── Beam
├── Projectile
├── OrbitingProjectiles
├── AutoProjectile
└── EnemyProjectile
```

### Key Classes

#### **Entity** (Base Class)
```cpp
class Entity {
    unsigned int id;
    std::string name;
    std::string image_path;
    pdcpp::Point<int> position;     // Pixel coordinates
    pdcpp::Point<int> tiledPosition;  // Tile coordinates
    pdcpp::Point<int> size;
    float hp, maxHP;
    LCDBitmap* bitmap;
    bool isFlashing;  // Damage indication
};
```
**Purpose**: Common properties for all game objects
**Key Methods**:
- `Draw()` - Renders entity and health bar
- `Damage(float)` - Handles damage with visual feedback
- `LoadBitmap()` - Loads sprite with caching

#### **Creature** (Extends Entity)
```cpp
class Creature : public Entity {
    int strength, agility, constitution;
    float evasion;
    unsigned int xp;
    int movementScale;
    Inventory* inventory;
    Weapon* equippedWeapon;
    Armor* equippedArmor;
};
```
**Purpose**: Adds combat stats and inventory to entities
**Key Methods**:
- `Attack(Creature*)` - Combat calculation with dodge chance
- `GetWeaponDamage()`, `GetArmorDefense()`

#### **Player** (Extends Creature)
```cpp
class Player : public Creature {
    unsigned int level, skillPoints;
    std::vector<std::unique_ptr<Magic>> magicLaunched;
    std::vector<SkillDefinition> skills;
    unsigned int monstersKilled, gameStartTime;
    bool isPlayerActive;  // For slowdown ability
};
```
**Purpose**: Player character with input handling and progression
**Key Methods**:
- `Tick(Area*)` - Update player state each frame
- `HandleInput()` - Process D-pad, crank, buttons
- `HandleAutoFire(Area*)` - Passive targeting system
- `AddXP(unsigned int)` - Experience and leveling
- `SpendSkillPoint(StatType)` - Stat allocation

#### **Monster** (Extends Creature)
```cpp
class Monster : public Creature {
    enum class MovementType { AStar, NoClip, Stationary, RangedKite };
    MovementType movementType;
    std::vector<pdcpp::Point<int>> path;
    bool pathFound;
    unsigned int lastAttackTime;
};
```
**Purpose**: Enemy AI and behavior
**Key Methods**:
- `Tick(Player*, Area*)` - AI decision making
- `HandleAStarMovement()` - Pathfinding navigation
- `HandleRangedKiteMovement()` - Keep distance while attacking
- `FireRangedAttack()` - Ranged combat

#### **Area** (Extends Entity)
```cpp
class Area : public Entity {
    std::vector<Layer> mapData;
    std::shared_ptr<MapCollision> collider;
    std::vector<std::shared_ptr<Monster>> livingMonsters;
    std::vector<std::shared_ptr<Monster>> toSpawnMonsters;
    std::shared_ptr<AStarContainer> pathfindingContainer;
    bool slowdownActive;  // Tactical slowdown ability
};
```
**Purpose**: Manages level, monsters, collision, and spawning
**Key Methods**:
- `Tick(Player*)` - Update monsters, handle spawning
- `ContinueMapGeneration()` - Incremental procedural generation
- `Render(int, int, int, int)` - Draw visible tiles and entities

#### **GameManager**
```cpp
class GameManager {
    std::unique_ptr<EntityManager> entityManager;
    std::shared_ptr<Player> player;
    std::shared_ptr<UI> ui;
    std::shared_ptr<Area> activeArea;
    pdcpp::Point<int> currentCameraOffset;
    bool isGameRunning;
    int maxScore;
};
```
**Purpose**: Top-level game state and coordination
**Key Methods**:
- `Update()` - Main game loop
- `LoadNewGame()`, `LoadSavedGame()` - Game initialization
- `SaveGame()` - Persist game state

---

## Memory Management

CardoBlast uses modern C++ smart pointers for safe memory management:

### Smart Pointer Strategy

#### `std::unique_ptr` - Single Ownership
```cpp
// GameManager owns these exclusively
std::unique_ptr<EntityManager> entityManager;
std::unique_ptr<AnimationClip> idle, run, attack;
std::vector<std::unique_ptr<Magic>> magicLaunched;
```
**Used for**: Resources with clear single owner

#### `std::shared_ptr` - Shared Ownership
```cpp
// Multiple systems need access to player
std::shared_ptr<Player> player;

// Monsters are shared between Area and EntityManager
std::shared_ptr<Monster> monster;
```
**Used for**: Resources accessed by multiple systems

#### Raw Pointers - Non-Owning References
```cpp
// EntityManager is owned by GameManager
void SetEntityManager(EntityManager* manager);

// Player passed to systems that don't own it
void Tick(Player* player, Area* area);
```
**Used for**: "I need to use this but don't own it"

### Ownership Model

```
GameManager (root owner)
├── entityManager (unique_ptr)
│   └── entities (map<id, shared_ptr<Entity>>)
├── player (shared_ptr) ────┐
├── activeArea (shared_ptr) │
│   └── monsters (vector<shared_ptr<Monster>>)
└── ui (shared_ptr)         │
    └── uses player* ────────┘ (non-owning reference)
```

### Critical Rules

1. **ImageTable Lifetime**: Animation resources must outlive AnimationClips
   ```cpp
   // CORRECT:
   std::unique_ptr<pdcpp::ImageTable> animationTable;  // Member variable
   idle->AddBitmap((*animationTable)[0]);  // Safe

   // WRONG:
   auto table = std::make_unique<pdcpp::ImageTable>(...);  // Local variable
   idle->AddBitmap((*table)[0]);  // Dangling pointer!
   ```

2. **Entity Manager Lifetime**: Must outlive all users
   - Owned by GameManager as `unique_ptr`
   - Passed to others as raw `EntityManager*`
   - Guaranteed to outlive all users

3. **Magic Object Lifetime**: Magic objects owned by Player
   ```cpp
   std::vector<std::unique_ptr<Magic>> magicLaunched;  // Player owns
   // Magic stores weak_ptr<Player> to avoid circular reference
   ```

---

## Core Systems

### 1. Entity Management System

**EntityManager** acts as a registry for all game entities:

```cpp
class EntityManager {
    std::map<unsigned int, std::shared_ptr<void>> data;
    std::shared_ptr<Player> player;
public:
    template <typename T>
    void LoadJSON(const char* fileName, int limitOfTokens);

    std::shared_ptr<void> GetEntity(unsigned int id);
};
```

**Responsibilities**:
- Load entities from JSON files
- Store entities in type-erased map
- Provide access to entities by ID
- Maintain reference to current player

**Data Loading Flow**:
```
JSON File → JSMN Parser → Entity::DecodeJson() → EntityManager::data
```

### 2. Collision System

**MapCollision** provides tile-based collision and pathfinding:

```cpp
class MapCollision {
    Map_Layer map;  // 2D array of collision flags
public:
    bool ComputePath(Point start, Point end, vector<Point>& path);
    void block(float x, float y, bool isCharacter);
    void unblock(float x, float y);
};
```

**Features**:
- A* pathfinding for monster navigation
- Dynamic blocking for monster positions
- Tile-based collision detection
- Connectivity validation for procedural maps

### 3. Procedural Generation System

**Area** implements incremental map generation to avoid frame drops:

```cpp
enum class GenerationStep {
    InitializeGrid,
    AddBoundaries,
    PlaceSimpleObstacles,
    PlaceStructuredObstacles,
    ValidateConnectivity,
    Complete
};
```

**Generation Process**:
1. **InitializeGrid**: Create empty grid
2. **AddBoundaries**: Add walls around edges
3. **PlaceSimpleObstacles**: Scatter random obstacles (5 per tick)
4. **PlaceStructuredObstacles**: Add L-shapes, T-shapes, walls (1 per tick)
5. **ValidateConnectivity**: Flood fill to ensure map is fully connected
6. **FixConnectivity**: Remove obstacles if map is disconnected

**Why Incremental?**
- Prevents frame drops during generation
- Shows loading screen with progress
- Can be interrupted if needed

### 4. Combat System

#### Melee Combat
```cpp
int Creature::Attack(Creature* target) {
    // Dodge calculation
    float dodgeChance = min(0.5f, (target->evasion + target->agility) * 0.001f);
    if (random roll < dodgeChance) return 0;  // Dodged!

    // Damage calculation
    int baseDamage = strength + weaponDamage;
    int defense = target->armor + target->constitution;
    int finalDamage = max(1, baseDamage - defense / 2);

    target->Damage(finalDamage);
    return finalDamage;
}
```

#### Ranged Combat
Monsters can fire projectiles based on movement type:
```cpp
void Monster::FireRangedAttack(Player* player, Area* area) {
    float angle = CalculateAngleToPlayer(player);
    float damage = baseDamage + (strength * multiplier);

    if (movementType == Stationary) {
        // Spread shot (5 projectiles)
        for (int i = 0; i < 5; i++) {
            area->CreateEnemyProjectile(position, angle + spread, ...);
        }
    } else {
        // Single projectile
        area->CreateEnemyProjectile(position, angle, ...);
    }
}
```

### 5. Magic System

**Magic** base class with specialized implementations:

```cpp
class Magic {
    std::weak_ptr<Player> player;  // Avoid circular reference
    bool isAlive;
    unsigned int bornTime, lifetime;
    pdcpp::Point<int> position;

    virtual void HandleInput();  // Crank aiming
    virtual void Damage(Area* area);  // Collision detection
    virtual void Draw() const;
};
```

**Spell Types**:
- **Beam**: Ray-cast damage in direction
- **Projectile**: Traveling projectile with explosion
- **OrbitingProjectiles**: Protective orbs that circle player
- **AutoProjectile**: Homing missile that targets nearest enemy

### 6. UI System

**UI** manages game screens and rendering:

```cpp
class UI {
    GameScreen currentScreen;  // LOADING, MAIN_MENU, GAME, GAME_OVER
    pdcpp::Font font;
    float loadingProgress;
    int selectedMenuItem;
    bool showLevelUpPopup;

    void DrawLoadingScreen() const;
    void DrawMainMenu() const;
    void DrawGameScreen() const;
    void DrawGameOverScreen() const;
    void DrawLevelUpPopup() const;
};
```

**Features**:
- pdcpp utilities for layout and text rendering
- Rounded rectangles for modern look
- Circular progress indicator for cooldowns
- Level-up popup for stat allocation
- Menu system with system menu integration

---

## Game Loop

### Main Loop (GameManager::Update)

```
┌─────────────────────────────────────────┐
│ Clear screen (kColorBlack)              │
└────────────────┬────────────────────────┘
                 │
                 ├─> if (!isGameRunning)
                 │   ├─> if (generating map)
                 │   │   └─> area->ContinueMapGeneration()
                 │   │       ├─> Update loading progress
                 │   │       └─> If complete → spawn player → isGameRunning = true
                 │   └─> else (loading JSON)
                 │       └─> Load entities incrementally
                 │
                 └─> if (isGameRunning)
                     ├─> Check if player leveled up
                     │   └─> Show level-up popup if needed
                     │
                     ├─> if (!showing level-up popup && player alive)
                     │   ├─> player->Tick(area)
                     │   │   ├─> HandleInput()
                     │   │   ├─> HandleAutoFire()
                     │   │   ├─> Move player
                     │   │   └─> Update magic projectiles
                     │   │
                     │   └─> area->Tick(player)
                     │       ├─> Update enemy AI
                     │       ├─> Spawn monsters
                     │       ├─> Check collisions
                     │       └─> Remove dead monsters
                     │
                     ├─> Update camera (smooth follow)
                     ├─> Set draw offset
                     ├─> area->Render(...)
                     ├─> player->Draw()
                     ├─> player->DrawMagic()
                     └─> ui->SetOffset(drawOffset)
```

### Update Frequency
- **Frame Rate**: 20 FPS (set in main.cpp via `setRefreshRate(20)`)
- **Monster Pathfinding**: Staggered across 10 groups per frame
- **Monster Spawning**: Every 60 ticks (3 seconds)
- **Auto-Fire**: 1000ms cooldown

---

## Data Flow

### Input Flow
```
Hardware Input → Playdate SDK → GameManager::Update()
                                    ↓
                               player->HandleInput()
                                    ↓
                    ┌───────────────┴───────────────┐
                    │                               │
              Movement (D-Pad)              Magic (Crank + Buttons)
                    │                               │
          player->Move(dx, dy, area)      Create Magic projectile
                    │                               │
            Check collision               Add to magicLaunched vector
```

### Combat Flow
```
Monster→Tick() → Calculate distance to player
                      ↓
              ┌───────┴────────┐
              │                │
        In melee range    In ranged range
              │                │
    player->Damage()    CreateEnemyProjectile()
                              ↓
                    EnemyProjectile→Update()
                              ↓
                    Collision with player?
                              ↓
                        player->Damage()
```

### Save/Load Flow
```
Save:
Player + Area → SaveGame::SerializePlayer()
              → SaveGame::SerializeArea()
              → JSON string
              → File::write()

Load:
File::read() → JSON string
             → jsmn parse
             → SaveGame::DeserializePlayer()
             → SaveGame::DeserializeArea()
             → Player + Area
```

---

## Design Patterns

### 1. Factory Pattern (Entity Creation)
```cpp
template <typename T>
void EntityManager::LoadJSON(const char* fileName, int limitOfTokens) {
    // Parse JSON
    // For each object in JSON:
    auto decoded = T::DecodeJson(buffer, tokens, size, this);
    // Store in registry
}
```

### 2. Observer Pattern (UI Callbacks)
```cpp
ui->SetOnNewGameSelected([this]() { LoadNewGame(); });
ui->SetOnSaveGameSelected([this]() { SaveGame(); });
```

### 3. Strategy Pattern (Monster Movement)
```cpp
switch (movementType) {
    case AStar:    HandleAStarMovement();    break;
    case NoClip:   HandleNoClipMovement();   break;
    case Stationary: /* stay in place */    break;
    case RangedKite: HandleRangedKiteMovement(); break;
}
```

### 4. Object Pool (Monsters)
```cpp
vector<shared_ptr<Monster>> bankOfMonsters;    // Templates
vector<shared_ptr<Monster>> toSpawnMonsters;  // Ready to spawn
vector<shared_ptr<Monster>> livingMonsters;   // Active in game

// Spawn new monster
Monster copy(*bankOfMonsters[randomIndex]);  // Copy constructor
livingMonsters.push_back(make_shared<Monster>(copy));
```

### 5. Component Pattern (Entity System)
```
Entity (base properties)
  └─> Creature (adds combat)
        └─> Player (adds input, magic)
        └─> Monster (adds AI)
```

---

## Performance Considerations

### 1. Rendering Optimizations
```cpp
// Field-of-view culling
bool visibleX = abs(cameraX - (tileX * tileWidth)) < FOV_X;
bool visibleY = abs(cameraY - (tileY * tileHeight)) < FOV_Y;
if (visibleX && visibleY) {
    DrawTile(x, y);
}
```

### 2. Pathfinding Optimization
```cpp
// Stagger pathfinding across frames
int staggerAmount = MONSTER_MAX_LIVING_COUNT;  // 10
monster->SetCanComputePath((monsterIndex % staggerAmount) == pathfindingTickCounter);
```
**Result**: Only 1 monster per frame computes path (instead of all 10)

### 3. Bitmap Caching
```cpp
static unordered_map<string, LCDBitmap*> bitmapCache;

void Entity::LoadBitmap() {
    auto it = bitmapCache.find(image_path);
    if (it != bitmapCache.end()) {
        bitmap = it->second;  // Reuse cached bitmap
        return;
    }
    // Load and cache new bitmap
}
```

### 4. Monster Collision Blocking
```cpp
// Block monsters from walking over each other
for (auto& monster : livingMonsters) {
    collider->block(monster->GetTiledPosition());
}

// Compute paths and move

// Unblock after movement
for (auto& monster : livingMonsters) {
    collider->unblock(monster->GetTiledPosition());
}
```

### 5. Incremental Map Generation
- Generates obstacles over multiple frames
- Prevents frame drops during generation
- Shows progress to player

---

## Configuration

All game constants are in `Globals.h`:

```cpp
namespace Globals {
    constexpr int MAP_TILE_SIZE = 16;
    constexpr int PLAYER_SIZE = 16;
    constexpr int MONSTER_MAX_LIVING_COUNT = 10;
    constexpr int MONSTER_TOTAL_TO_SPAWN = 50;
    constexpr float AUTO_FIRE_RANGE = 100.0f;
    // ... 40+ more constants
}
```

**Benefits**:
- Single source of truth
- Easy to balance gameplay
- No magic numbers in code
- Compile-time constants (zero runtime cost)

---

## Future Architecture Improvements

### Potential Enhancements
1. **Event System**: Decouple systems with event bus
2. **Component System**: More granular entity components
3. **State Machine**: For complex AI behaviors
4. **Resource Manager**: Centralized asset loading
5. **Spatial Partitioning**: Quadtree for collision detection

### Technical Debt
- Magic system could use more abstraction
- Some long functions could be broken down
- More unit tests for core systems

---

**Last Updated**: 2026-01-23
**Maintainer**: Sergio Prada
