# CardoBlast Game Systems Documentation

This document provides detailed explanations of all major game systems in CardoBlast.

## Table of Contents
1. [Player System](#player-system)
2. [Enemy AI System](#enemy-ai-system)
3. [Combat System](#combat-system)
4. [Magic System](#magic-system)
5. [Procedural Generation System](#procedural-generation-system)
6. [Progression System](#progression-system)
7. [Spawning System](#spawning-system)
8. [Camera System](#camera-system)
9. [Save System](#save-system)
10. [UI System](#ui-system)

---

## Player System

### Overview
The Player system handles character control, animation, abilities, and stats.

### Components

#### Movement
- **Speed**: Configurable via `Globals::PLAYER_MOVEMENT_SPEED` (default: 5.0f)
- **Controls**: 8-directional movement using D-pad
- **Collision**: Checks map collision before moving
- **Animation States**: idle, run, attack, stab, die

```cpp
void Player::Move(int deltaX, int deltaY, const Area& area) {
    if (area->CheckCollision(x + 8, y + 10)) return;  // Foot collision
    SetPosition(Point(x, y));
}
```

#### Input Handling
- **D-Pad**: Movement in 8 directions
- **Crank**: Aim magic spells (0-360 degrees)
- **A Button**: Cycle magic forward
- **B Button**: Cycle magic backward
- **Holding Buttons**: Auto-cast active spell

#### Activity Tracking
Player activity affects the **Tactical Slowdown** ability:
```cpp
// Player is "active" if moving or rotating crank
bool movementInput = (clicked & kButtonUp | kButtonDown | kButtonLeft | kButtonRight);
bool crankRotating = (crankChange > 0.5f || crankChange < -0.5f);

if (movementInput || crankRotating) {
    isPlayerActive = true;
} else if (idleTime > 500ms) {
    isPlayerActive = false;  // Triggers slowdown ability
}
```

### Stats System

#### Core Stats
- **HP / Max HP**: Health points
- **Strength**: Increases magic damage
- **Agility**: Increases dodge chance
- **Constitution**: Reduces incoming damage
- **Evasion**: Base dodge chance

#### Stat Allocation
Players earn 1 skill point per level:
```cpp
enum class StatType { Strength, Agility, Constitution, MaxHp };

bool SpendSkillPoint(StatType type) {
    if (skillPoints == 0) return false;

    switch (type) {
        case Strength:     strength++;        break;
        case Agility:      agility++;         break;
        case Constitution: constitution++;    break;
        case MaxHp:        maxHP += 5;        break;
    }
    skillPoints--;
    return true;
}
```

---

## Enemy AI System

### Movement Types

CardoBlast features 4 distinct AI behaviors:

#### 1. AStar (Pathfinding)
- Uses A* algorithm to navigate around obstacles
- Most common enemy type
- Chases player intelligently

```cpp
void Monster::HandleAStarMovement(Point playerPos, Area* area) {
    if (needsNewPath) {
        path = area->GetCollider()->ComputePath(position, playerPos);
        pathFound = true;
    }

    if (pathFound && !path.empty()) {
        MoveTowards(path.back());
        if (ReachedNode()) path.pop_back();
    }
}
```

#### 2. NoClip (Ghosting)
- Moves directly toward player, ignoring obstacles
- Faster and more aggressive
- No pathfinding overhead

```cpp
void Monster::HandleNoClipMovement(Point playerPos) {
    Vector direction = Normalize(playerPos - position);
    position += direction * movementSpeed;
}
```

#### 3. Stationary (Turret)
- Doesn't move
- Fires spread shots (5 projectiles)
- Higher damage output
- Strategic positioning challenges

#### 4. RangedKite (Hit-and-Run)
- Maintains distance from player (3-7 tiles)
- Fires single projectiles
- Retreats when too close, advances when too far
- Most tactically complex enemy

```cpp
void Monster::HandleRangedKiteMovement(Point playerPos, Area* area) {
    float distance = position.distance(playerPos);

    if (distance < KITE_MIN_RANGE) {
        // Too close - retreat
        Point retreatTarget = GetKiteTarget(playerPos, area);
        MoveAway(retreatTarget);
    } else if (distance > KITE_MAX_RANGE) {
        // Too far - advance
        MoveTowards(playerPos);
    } else {
        // Perfect range - stay put and shoot
        FireRangedAttack(player, area);
    }
}
```

### Pathfinding Optimization

**Problem**: Computing paths for 10 monsters every frame is expensive.

**Solution**: Stagger pathfinding across frames:
```cpp
// Each frame, only 1 monster computes a path
int staggerAmount = 10;
bool canComputePath = (monsterIndex % staggerAmount) == currentTick;

if (canComputePath && needsNewPath) {
    ComputePath();
}
```

**Result**: 10x performance improvement

### Attack Behavior

#### Melee Attacks
- Trigger when within 2 tiles of player
- Direct damage to player
- No cooldown

#### Ranged Attacks
**Ranged Kite** (single shot):
- Cooldown: 2000ms
- Range: 15 tiles
- Damage: `base (0.5) + strength * 0.2`

**Stationary** (spread shot):
- Cooldown: 4000ms
- Range: 15 tiles
- Projectiles: 5 (spread 45°)
- Damage: `base (1.0) + strength * 0.3`

---

## Combat System

### Damage Calculation

#### Melee Combat (Monster → Player)
```cpp
int Creature::Attack(Creature* target) {
    // 1. Dodge Check
    float dodgeChance = min(0.5, (target->evasion + target->agility) * 0.001);
    if (RandomRoll() < dodgeChance) return 0;  // Attack missed!

    // 2. Damage Calculation
    int baseDamage = strength + weaponDamage;
    int defense = target->armorDefense + target->constitution;
    int finalDamage = max(1, baseDamage - defense / 2);

    target->Damage(finalDamage);
    return finalDamage;
}
```

#### Magic Combat (Player → Monster)
Each magic type has different damage:
- **Beam**: Continuous damage (damage per tick)
- **Projectile**: Single hit + explosion AOE
- **Orbiting**: Contact damage
- **Auto-Fire**: Homing projectile

Damage scales with player strength:
```cpp
float magicDamage = baseDamage * (1.0 + strength * 0.1);
```

#### Projectile Combat (Enemy → Player)
```cpp
void EnemyProjectile::CheckCollision(Player* player) {
    if (position.distance(player->position) < size) {
        player->Damage(damage);
        isAlive = false;  // Projectile despawns
    }
}
```

### Visual Feedback
Entities flash when damaged:
```cpp
void Entity::Damage(float damage) {
    hp -= damage;
    isFlashing = true;
    flashTimer = 0;
}

bool Entity::CalculateFlashing() {
    if (isFlashing) {
        flashTimer++;
        isBitmapVisible = !isBitmapVisible;  // Toggle visibility

        if (flashTimer > MAX_FLASHING_TICKS) {
            isFlashing = false;
            isBitmapVisible = true;
        }
    }
    return isBitmapVisible;
}
```

---

## Magic System

### Spell Architecture

All spells inherit from base `Magic` class:
```cpp
class Magic {
protected:
    weak_ptr<Player> player;    // Owner reference
    bool isAlive;
    unsigned int bornTime;
    unsigned int lifetime;      // Spell duration
    Point position;

public:
    virtual void HandleInput();  // Crank aiming
    virtual void Damage(Area* area);  // Hit detection
    virtual void Draw() const;
};
```

### Spell Types

#### 1. Beam
**Type**: Ray-cast continuous damage
**Cooldown**: 2000ms
**Range**: 200 pixels

```cpp
void Beam::Damage(Area* area) {
    float angle = GetCrankAngle();
    Point direction = {cos(angle), sin(angle)};

    // Ray-cast to find hit monsters
    for (int i = 0; i < range; i += stepSize) {
        Point rayPos = position + direction * i;

        for (auto& monster : area->GetMonsters()) {
            if (rayPos.distance(monster->position) < hitRadius) {
                monster->Damage(damagePerTick);
            }
        }
    }
}
```

#### 2. Projectile
**Type**: Traveling projectile with explosion
**Cooldown**: 1500ms
**Explosion**: AOE damage at end

```cpp
void Projectile::Update(Area* area) {
    // Move forward
    position += velocity;

    // Check if exploding
    if (elapsedTime > explosionThreshold || CollisionDetected()) {
        exploding = true;
        size += sizeIncrement;  // Grow explosion radius

        // Damage all monsters in radius
        for (auto& monster : area->GetMonsters()) {
            if (position.distance(monster->position) < size) {
                monster->Damage(explosionDamage);
            }
        }
    }

    if (elapsedTime > lifetime) isAlive = false;
}
```

#### 3. Orbiting Projectiles
**Type**: Protective orbs circling player
**Cooldown**: 5000ms
**Duration**: 10 seconds

```cpp
void OrbitingProjectiles::Update(Area* area) {
    // Update each orb's angle
    for (int i = 0; i < orbCount; i++) {
        float angle = currentAngle + (i * 2 * PI / orbCount);
        orbPositions[i] = playerPos + Point(cos(angle), sin(angle)) * radius;

        // Check collision with monsters
        for (auto& monster : area->GetMonsters()) {
            if (orbPositions[i].distance(monster->position) < orbSize) {
                monster->Damage(contactDamage);
            }
        }
    }
    currentAngle += rotationSpeed;
}
```

#### 4. Auto-Fire (Passive)
**Type**: Homing missile
**Cooldown**: 1000ms (always active)
**Range**: 100 pixels

```cpp
void Player::HandleAutoFire(Area* area) {
    if (cooldownElapsed < autoFireCooldown) return;

    // Find nearest enemy
    Monster* closest = nullptr;
    float minDist = AUTO_FIRE_RANGE;

    for (auto& monster : area->GetMonsters()) {
        float dist = position.distance(monster->position);
        if (dist < minDist) {
            minDist = dist;
            closest = monster;
        }
    }

    if (closest) {
        auto projectile = make_unique<AutoProjectile>(position, closest);
        magicLaunched.push_back(move(projectile));
    }
}
```

#### 5. Rapid Shot
**Type**: Fast, low-damage projectiles
**Cooldown**: 800ms
**Damage**: 0.25x (25% of normal)

#### 6. Heavy Beam
**Type**: Powerful beam with extended range
**Cooldown**: 3500ms
**Damage**: 0.7x per tick (70% but continuous)
**Range**: 500 pixels

### Spell Management

Player owns all active spells:
```cpp
class Player {
    vector<unique_ptr<Magic>> magicLaunched;

    void Tick(Area* area) {
        // Update all active spells
        for (auto& magic : magicLaunched) {
            if (magic->IsAlive()) {
                magic->Update(area);
            } else {
                magic.reset();  // Remove dead spell
            }
        }

        // Clean up null pointers
        magicLaunched.erase(
            remove(magicLaunched.begin(), magicLaunched.end(), nullptr),
            magicLaunched.end()
        );
    }
};
```

---

## Procedural Generation System

### Generation Algorithm

Maps are generated incrementally to prevent frame drops:

#### Step 1: Initialize Grid
```cpp
void InitializeGrid(int width, int height) {
    for (int i = 0; i < width * height; i++) {
        tiles[i] = {id: 1, collision: false};  // Walkable tile
    }
}
```

#### Step 2: Add Boundaries
```cpp
void AddBoundaries() {
    for (int x = 0; x < width; x++) {
        tiles[x] = {id: 2, collision: true};              // Top
        tiles[width * (height-1) + x] = {id: 2, collision: true};  // Bottom
    }
    for (int y = 0; y < height; y++) {
        tiles[y * width] = {id: 2, collision: true};      // Left
        tiles[y * width + (width-1)] = {id: 2, collision: true};  // Right
    }
}
```

#### Step 3: Place Simple Obstacles
Place 5 obstacles per frame to avoid lag:
```cpp
void PlaceSimpleObstacles() {
    int totalObstacles = width * height * obstacleDensity;  // 15% of tiles

    for (int i = 0; i < 5 && placed < totalObstacles; i++) {
        int x = random(2, width - 3);
        int y = random(2, height - 3);
        int size = random(minSize, maxSize);

        if (CanPlaceObstacle(x, y, size)) {
            PlaceObstacle(x, y, size);
            placed++;
        }
    }
}
```

#### Step 4: Place Structured Obstacles
Add interesting shapes (1 per frame):
```cpp
void PlaceStructuredObstacle() {
    int shapeType = random(0, 4);

    switch (shapeType) {
        case 0: PlaceLShape(x, y);      break;  // ┗
        case 1: PlaceTShape(x, y);      break;  // ┻
        case 2: PlaceWall(x, y);        break;  // ──
        case 3: PlacePlatform(x, y);    break;  // Square
        case 4: PlacePillar(x, y);      break;  // Single tile
    }
}
```

#### Step 5: Validate Connectivity
Ensure all walkable tiles are connected:
```cpp
bool ValidateConnectivity() {
    // Flood fill from center
    vector<Point> visited;
    queue<Point> toVisit;
    toVisit.push({width/2, height/2});

    while (!toVisit.empty()) {
        Point current = toVisit.front();
        toVisit.pop();

        // Check 4-directional neighbors
        for (Point neighbor : GetNeighbors(current)) {
            if (IsWalkable(neighbor) && !IsVisited(neighbor)) {
                toVisit.push(neighbor);
                visited.push_back(neighbor);
            }
        }
    }

    // Check if all walkable tiles were reached
    int walkableTiles = CountWalkableTiles();
    return visited.size() == walkableTiles;
}
```

#### Step 6: Fix Connectivity
Remove obstacles to connect isolated areas:
```cpp
void FixConnectivity() {
    // Try removing obstacles near center first
    RemoveObstaclesInRadius(centerX, centerY, radius: 3);

    if (!ValidateConnectivity()) {
        // Iterate through all tiles, removing obstacles one at a time
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                if (IsObstacle(x, y)) {
                    RemoveObstacle(x, y);
                    if (ValidateConnectivity()) return;  // Fixed!
                    AddObstacle(x, y);  // Didn't help, restore
                }
            }
        }
    }
}
```

### Generation Parameters
```cpp
struct GenerationParams {
    int width = 40;
    int height = 40;
    float obstacleDensity = 0.15f;        // 15% of tiles
    int minObstacleSize = 1;
    int maxObstacleSize = 3;
    int minStructuredObstacles = 3;
    int maxStructuredObstacles = 8;
};
```

### Progress Tracking
```cpp
float GetGenerationProgress() {
    switch (currentStep) {
        case InitializeGrid:           return 0.05f;
        case AddBoundaries:            return 0.10f;
        case PlaceSimpleObstacles:
            return 0.10f + 0.30f * (placed / target);
        case PlaceStructuredObstacles:
            return 0.40f + 0.30f * (placed / target);
        case ValidateConnectivity:     return 0.70f;
        case FixConnectivity:          return 0.80f + 0.20f * progress;
        case Complete:                 return 1.00f;
    }
}
```

---

## Progression System

### Experience Formula
```cpp
unsigned int GetXPToLevelUp(unsigned int currentLevel) {
    return XP_BASE + (currentLevel * currentLevel * XP_FACTOR);
    // Level 0→1: 10 XP
    // Level 1→2: 15 XP
    // Level 2→3: 30 XP
    // Level 3→4: 55 XP
    // etc.
}
```

### Leveling System
```cpp
void Player::AddXP(unsigned int amount) {
    xp += amount;

    while (xp >= GetXPToLevelUp(level)) {
        xp -= GetXPToLevelUp(level);
        LevelUp();
    }
}

bool Player::LevelUp() {
    level++;
    skillPoints++;
    SetHP(maxHP);  // Full heal on level up
    justLeveledUp = true;  // Trigger UI popup
    return true;
}
```

### Skill Point System
- 1 skill point per level
- Can be allocated to 4 stats:
  - **+1 Strength**: Increases damage
  - **+1 Agility**: Increases dodge
  - **+1 Constitution**: Reduces damage taken
  - **+5 Max HP**: Increases survivability

### Level-Up Popup
```cpp
void UI::ShowLevelUpPopup() {
    showLevelUpPopup = true;
    pendingLevelUps++;

    // Game pauses until player allocates point
    // Player chooses stat with D-pad
    // Confirms with A button
}
```

---

## Spawning System

### Spawn Mechanics

#### Wave System
```cpp
// Spawn up to 10 living monsters at a time
// Total of 50 monsters per game
constexpr int MAX_LIVING = 10;
constexpr int TOTAL_TO_SPAWN = 50;
constexpr int TICKS_BETWEEN_SPAWNS = 60;  // 3 seconds at 20 FPS

void Area::SpawnCreature() {
    if (ticksSinceLastSpawn < TICKS_BETWEEN_SPAWNS) {
        ticksSinceLastSpawn++;
        return;
    }

    if (livingMonsters.size() >= MAX_LIVING) return;
    if (monstersSpawnedCount >= TOTAL_TO_SPAWN) return;

    // Spawn new monster
    Point spawnPos = FindSpawnablePosition();
    Monster* monster = toSpawnMonsters.front();
    monster->SetPosition(spawnPos);
    livingMonsters.push_back(monster);
    toSpawnMonsters.erase(toSpawnMonsters.begin());

    ticksSinceLastSpawn = 0;
    monstersSpawnedCount++;
}
```

#### Spawn Position Selection
```cpp
Point Area::FindSpawnablePosition(int attempt) {
    if (attempt >= MAX_SPAWN_ATTEMPTS) return {0, 0};

    // Random position from pre-computed spawnable tiles
    Point pos = spawnablePositions[random() % spawnablePositions.size()];

    // Must be far from player (20+ tiles)
    if (pos.distance(player->GetTiledPosition()) < SPAWN_RADIUS) {
        return FindSpawnablePosition(attempt + 1);  // Try again
    }

    return pos;
}
```

#### Spawnable Position Caching
```cpp
void Area::LoadSpawnablePositions() {
    spawnablePositions.clear();

    // Pre-compute all walkable tiles
    for (int i = 0; i < width * height; i++) {
        if (!mapData[0].tiles[i].collision) {
            int x = i % width;
            int y = i / width;
            spawnablePositions.push_back({x, y});
        }
    }
}
```

### Monster Selection
Monsters are randomly selected from the bank:
```cpp
void Area::SetupMonstersToSpawn() {
    for (int i = 0; i < TOTAL_TO_SPAWN; i++) {
        int randomIndex = random() % bankOfMonsters.size();
        Monster copy = *bankOfMonsters[randomIndex];  // Copy constructor
        toSpawnMonsters.push_back(make_shared<Monster>(copy));
    }
}
```

---

## Camera System

### Smooth Follow
```cpp
void GameManager::UpdateCamera() {
    float cameraSpeed = 0.2f;  // Lower = smoother, higher = more responsive

    // Interpolate camera position
    currentCameraOffset.x += (player->position.x - currentCameraOffset.x) * cameraSpeed;
    currentCameraOffset.y += (player->position.y - currentCameraOffset.y) * cameraSpeed;
}
```

### Screen Bounds
```cpp
void GameManager::CalculateDrawOffset() {
    int screenCenterX = SCREEN_WIDTH / 2;
    int screenCenterY = SCREEN_HEIGHT / 2;

    Point drawOffset = {0, 0};

    // Horizontal bounds
    if (cameraOffset.x > screenCenterX) {
        int maxX = (area->GetWidth() * TILE_SIZE) - screenCenterX;
        if (cameraOffset.x > maxX) {
            drawOffset.x = -(area->GetWidth() * TILE_SIZE - SCREEN_WIDTH);
        } else {
            drawOffset.x = -cameraOffset.x + screenCenterX;
        }
    }

    // Vertical bounds (same logic)
    // ...

    pd->graphics->setDrawOffset(drawOffset.x, drawOffset.y);
}
```

### Field of View Culling
Only render visible entities:
```cpp
void Area::Render(int cameraX, int cameraY, int fovX, int fovY) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int tileX = x * TILE_SIZE;
            int tileY = y * TILE_SIZE;

            bool visibleX = abs(cameraX - tileX) < fovX;
            bool visibleY = abs(cameraY - tileY) < fovY;

            if (visibleX && visibleY) {
                DrawTile(x, y);
            }
        }
    }
}
```

---

## Save System

### Save Data Structure
```json
{
  "version": 1,
  "player": {
    "position": { "x": 320, "y": 320 },
    "hp": 85.5,
    "maxHp": 100.0,
    "monstersKilled": 12,
    "survivalTime": 145,
    "level": 3,
    "xp": 2500,
    "strength": 15,
    "agility": 12,
    "constitution": 10,
    "evasion": 0.25,
    "skillPoints": 0
  },
  "area": {
    "id": 9002,
    "monstersSpawned": 25,
    "mapData": { ... }  // Procedural map
  }
}
```

### Serialization
```cpp
string SaveGame::SerializePlayer(shared_ptr<Player> player) {
    ostringstream json;
    json << "  \"player\": {\n";
    json << "    \"position\": { \"x\": " << player->GetPosition().x
         << ", \"y\": " << player->GetPosition().y << " },\n";
    json << "    \"hp\": " << player->GetHP() << ",\n";
    json << "    \"maxHp\": " << player->GetMaxHP() << ",\n";
    json << "    \"monstersKilled\": " << player->GetMonstersKilled() << ",\n";
    // ... more fields
    json << "  }";
    return json.str();
}
```

### Deserialization
```cpp
bool SaveGame::DeserializePlayer(shared_ptr<Player> player, const char* json) {
    jsmn_parser parser;
    jsmn_init(&parser);

    // Parse JSON
    jsmntok_t tokens[256];
    int tokenCount = jsmn_parse(&parser, json, strlen(json), tokens, 256);

    // Extract values
    if (const char* val = FindValue(json, tokens, "hp")) {
        player->SetHP(atof(val));
    }
    if (const char* val = FindValue(json, tokens, "position")) {
        // Parse nested object...
    }
    // ... more fields

    return true;
}
```

For complete save system documentation, see [SAVE_SYSTEM_README.md](SAVE_SYSTEM_README.md).

---

## UI System

### Screen States
```cpp
enum class GameScreen {
    LOADING,     // Initial JSON loading + map generation
    MAIN_MENU,   // New game / Load game
    GAME,        // Active gameplay
    GAME_OVER,   // Death screen with stats
    PAUSE        // System menu (not a full screen)
};
```

### Screen Rendering

#### Loading Screen
- Displays game title
- Shows loading progress bar
- Updates during JSON loading and map generation

#### Main Menu
- Title display
- Menu options (New Game, Load Game)
- Selection indicator

#### Game Screen
- Player stats (HP, level, XP)
- Magic cooldown indicator
- Kill counter
- Survival time
- Active magic icon

#### Game Over Screen
- Final stats (kills, time, score)
- Max score comparison
- Return to menu prompt

### Level-Up Popup
Pauses game and shows stat allocation UI:
```cpp
void UI::DrawLevelUpPopup() const {
    // Semi-transparent overlay
    Graphics::fillRect(screen, Colors::transparent50GrayB);

    // Popup panel
    Rectangle panel = screen.reduced(60, 60);
    Graphics::fillRoundedRect(panel, 8, Colors::black);
    Graphics::drawRoundedRect(panel, 8, 2, Colors::white);

    // Title
    font.drawText("Level Up!", panel.centerX(), panel.y + 20);

    // Stat options
    const char* options[] = {"+1 STR", "+1 AGI", "+1 CON", "+5 HP"};
    for (int i = 0; i < 4; i++) {
        int y = panel.y + 60 + i * 30;

        if (i == selectedOption) {
            Graphics::fillRect({panel.x + 10, y, 200, 25}, Colors::white);
            font.drawText(options[i], panel.x + 20, y + 5, Colors::black);
        } else {
            font.drawText(options[i], panel.x + 20, y + 5, Colors::white);
        }
    }
}
```

### Cooldown Indicator
Circular progress bar for active spell:
```cpp
class CircularProgress {
public:
    void Draw(Point center, float progress) {
        int radius = 16;
        float startAngle = -90;  // Start at top
        float endAngle = startAngle + (progress * 360);

        // Background circle
        Graphics::drawEllipse({center.x - radius, center.y - radius,
                              radius*2, radius*2}, 2, 0, 360, Colors::white);

        // Progress arc
        Graphics::fillEllipse({center.x - radius, center.y - radius,
                              radius*2, radius*2}, startAngle, endAngle,
                              Colors::steppedDither3);
    }
};
```

---

## Tactical Slowdown Ability

### Concept
When player stops moving, enemies gradually slow down for strategic planning.

### Implementation
```cpp
void Area::Tick(Player* player) {
    playerIsActive = player->IsPlayerActive();
    unsigned int currentTime = GetCurrentTime();

    if (playerIsActive) {
        // Player moving - deactivate slowdown
        slowdownActive = false;
        playerIdleTime = 0.0f;
    } else {
        // Player idle - check cooldown
        unsigned int timeSinceLastUse = currentTime - lastSlowdownActivationTime;
        bool cooldownReady = (timeSinceLastUse >= 10000);  // 10 seconds

        if (!slowdownActive && cooldownReady) {
            slowdownActive = true;
            lastSlowdownActivationTime = currentTime;
        }

        if (slowdownActive) {
            playerIdleTime = (currentTime - lastActivityCheckTime) / 1000.0f;
        }
    }
}
```

### Slowdown Curve
```cpp
float GetSlowdownMultiplier(float idleTime) {
    if (idleTime < 0.5f) {
        return 0.1f;  // Initial: 10% speed
    } else if (idleTime < 2.5f) {
        // Progressive speed-up over 2 seconds
        float progress = (idleTime - 0.5f) / 2.0f;
        return 0.1f + (progress * 0.9f);  // 10% → 100%
    } else {
        return 1.0f;  // Back to normal after 2.5s
    }
}
```

### Application
```cpp
void Monster::Move(Point target, Area* area) {
    int moveSpeed = GetMovementSpeed();

    if (area->IsSlowdownActive()) {
        float slowdownFactor = area->GetSlowdownMultiplier();
        moveSpeed = max(1, (int)(moveSpeed * slowdownFactor));
    }

    // Apply movement...
}
```

---

## Performance Metrics

### Target Performance
- **Frame Rate**: 20 FPS (50ms per frame)
- **Map Generation**: < 3 seconds
- **Spawn Rate**: 1 monster per 3 seconds
- **Max Living Monsters**: 10
- **Pathfinding**: 1 monster per frame

### Optimization Techniques
1. **Field-of-View Culling**: Only render visible entities
2. **Staggered Pathfinding**: Distribute AI computation
3. **Bitmap Caching**: Reuse loaded sprites
4. **Incremental Generation**: Spread work across frames
5. **Position Caching**: Pre-compute spawnable locations

---

**Last Updated**: 2026-01-23
**Maintainer**: Sergio Prada
