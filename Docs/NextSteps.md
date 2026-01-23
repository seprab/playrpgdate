# CardoBlast - Development Roadmap

This document outlines planned features, improvements, and ideas for future development.

## Table of Contents
1. [High Priority](#high-priority)
2. [Medium Priority](#medium-priority)
3. [Low Priority](#low-priority)
4. [Technical Improvements](#technical-improvements)
5. [Community Ideas](#community-ideas)
6. [Long-Term Vision](#long-term-vision)

---

## High Priority

### Core Gameplay Enhancements

#### Sound and Music System
- [ ] Background music for different screens (menu, game, game over)
- [ ] Sound effects for:
  - [ ] Magic spell casting
  - [ ] Player damage/death
  - [ ] Monster damage/death
  - [ ] Level up
  - [ ] Button navigation
- [ ] Volume control in system menu
- [ ] Audio fade in/out for transitions

#### Visual Effects
- [ ] Particle system for magic impacts
- [ ] Screen shake on heavy hits
- [ ] Better death animations for monsters
- [ ] Explosion effects for projectiles
- [ ] Damage numbers floating above entities
- [ ] Visual feedback for critical hits

#### UI Improvements
- [ ] Minimap in corner showing:
  - [ ] Player position
  - [ ] Monster locations
  - [ ] Map layout
- [ ] Better magic cooldown visualization
- [ ] Combo counter for consecutive kills
- [ ] Damage dealt counter
- [ ] Achievement notifications
- [ ] Clearer stat tooltips in level-up screen

---

## Medium Priority

### Progression Systems

#### Equipment System
- [ ] Weapon drops from monsters
  - [ ] Different weapon types (staff, wand, orb)
  - [ ] Stat bonuses (damage, speed, range)
  - [ ] Special effects (life steal, pierce, chain lightning)
- [ ] Armor system
  - [ ] Different armor pieces (robe, cape, amulet)
  - [ ] Defense bonuses
  - [ ] Set bonuses
- [ ] Inventory management UI
- [ ] Equipment comparison tooltips

#### Achievement System
- [ ] Achievements for:
  - [ ] Survival milestones (5min, 10min, 15min+)
  - [ ] Kill counts (10, 50, 100, 500)
  - [ ] Perfect level ups (no damage taken)
  - [ ] All spells unlocked
  - [ ] Defeat all monster types
  - [ ] Speedrun challenges
- [ ] Achievement notification system
- [ ] Rewards for achievements (unlockable content?)

#### Unlockable Content
- [ ] New magic spells unlocked by achievements
- [ ] New character skins
- [ ] Challenge modes (hard mode, endless, boss rush)
- [ ] Playable characters with different abilities

### Monster Variety

#### New Enemy Types
- [ ] **Flying enemies**: Ignore obstacles, move in arcs
- [ ] **Tank enemies**: High HP, slow, heavy melee damage
- [ ] **Swarm enemies**: Weak but spawn in groups
- [ ] **Elite variants**: Stronger versions with special abilities
- [ ] **Boss enemies**: Large HP pools, multiple phases, unique mechanics

#### Enemy Abilities
- [ ] **Teleporting enemies**: Blink around player
- [ ] **Summoner enemies**: Spawn other monsters
- [ ] **Shield enemies**: Block damage from one direction
- [ ] **Healer enemies**: Restore HP to nearby monsters
- [ ] **Explosive enemies**: Detonate on death

### Map Features

#### Biomes
- [ ] Forest biome (current aesthetic)
- [ ] Desert biome (sand, cacti, different color palette)
- [ ] Cave biome (darker, tighter corridors)
- [ ] Ice biome (slippery movement?)
- [ ] Corrupted biome (end-game area)

#### Special Rooms
- [ ] **Treasure rooms**: Guaranteed equipment drops
- [ ] **Boss arenas**: Circular rooms with boss fights
- [ ] **Safe zones**: No enemy spawns, healing over time
- [ ] **Challenge rooms**: Extra enemies for better rewards
- [ ] **Shrine rooms**: Temporary buffs or permanent upgrades

#### Environmental Hazards
- [ ] Lava/poison pools (damage over time)
- [ ] Spike traps
- [ ] Crumbling floors
- [ ] Moving platforms
- [ ] Teleporters between areas

---

## Low Priority

### Story Mode
- [ ] Narrative framework
- [ ] NPC dialogue system
- [ ] Quest system
- [ ] Multiple areas to progress through
- [ ] Story-driven boss fights
- [ ] Ending cutscenes

**Story Ideas** (from community):
> "Between levels, there could be a monster that has good things, and when the protagonist of the game takes the good things, he will have fortune, and when he takes the bad things from the monsters, he will have bad fortune. For example, if a poison monster has an apple and the hero takes it and eats it, he gets poisoned and loses."

> "The hero could have a bag where he keeps the deepest secrets of the forest and protects himself from the things outside his house."

### Meta-Progression
- [ ] Persistent upgrades between runs
- [ ] Unlock new starting equipment
- [ ] Unlock new starting stats
- [ ] Currency system for meta upgrades
- [ ] Difficulty modifiers for extra rewards

### Polish
- [ ] Tutorial mode for new players
- [ ] Pause menu improvements
- [ ] Better transition effects between screens
- [ ] Loading screen tips/lore
- [ ] Credits screen
- [ ] Options menu (volume, difficulty, etc.)

---

## Technical Improvements

### Code Quality
- [x] Standardize code formatting
- [x] Extract magic numbers to constants
- [x] Remove dead code
- [ ] Add unit tests for critical systems
- [ ] Add integration tests for game flow
- [ ] Profile and optimize hot paths
- [ ] Memory leak detection

### Architecture
- [ ] Event system for decoupled communication
- [ ] Component-based entity system
- [ ] State machine for complex AI
- [ ] Resource manager for asset loading
- [ ] Spatial partitioning (quadtree) for collision

### Performance
- [ ] Chunk-based map rendering for larger maps
- [ ] Object pooling for projectiles
- [ ] Optimize pathfinding for more enemies
- [ ] Batch sprite rendering
- [ ] Reduce memory allocations per frame

### Tools and Workflow
- [ ] Map editor integration
- [ ] Enemy designer tool (configure stats, behaviors)
- [ ] Animation frame editor
- [ ] Debug console (god mode, spawn enemies, etc.)
- [ ] Performance overlay (FPS, memory, entity count)

---

## Community Ideas

This section captures creative ideas from the community and contributors.

### From Sergio's Son
> "Between each level there could be a monster that has good things and bad things. When the hero takes the good things, he'll have fortune. When he takes bad things, he'll have bad fortune. For example, if a poison monster has an apple and the hero eats it, he gets poisoned and loses."

**Implementation Ideas**:
- Random events between waves
- Risk/reward mechanics (cursed items with powerful effects)
- "Treasure goblin" enemies that drop loot but flee

> "The hero should have a bag where he keeps the deepest secrets of the forest and protects himself from things outside his house."

**Implementation Ideas**:
- Inventory system for collecting lore items
- Collectible story fragments
- Item-based progression (collect keys to unlock areas)

### Potential Community Features
- [ ] Daily challenges (fixed seed, leaderboard)
- [ ] Seed sharing for interesting maps
- [ ] Replay system to share runs
- [ ] Co-op mode (if feasible on Playdate)
- [ ] Custom mode with configurable rules

---

## Long-Term Vision

### Phase 1: Core Polish (Current)
- [x] Stable gameplay loop
- [x] Multiple enemy types
- [x] Progression system
- [x] Save/load functionality
- [ ] Sound and music
- [ ] Visual effects

### Phase 2: Content Expansion
- [ ] Equipment system
- [ ] More enemy types and bosses
- [ ] Multiple biomes
- [ ] Special rooms and events
- [ ] Achievement system

### Phase 3: Replayability
- [ ] Meta-progression
- [ ] Challenge modes
- [ ] Daily runs
- [ ] Seed sharing
- [ ] Unlockable characters

### Phase 4: Story (Optional)
- [ ] Narrative framework
- [ ] Quest system
- [ ] NPCs and dialogue
- [ ] Multiple endings
- [ ] Lore items and worldbuilding

### Phase 5: Community
- [ ] Modding support (if possible)
- [ ] Level editor
- [ ] Steam/itch.io release (if applicable)
- [ ] Community challenges and events

---

## Notes and Considerations

### Design Philosophy
- **Keep it simple**: Playdate has limited controls, don't over-complicate
- **Bite-sized sessions**: Optimize for 5-15 minute play sessions
- **Clear feedback**: Visual and audio cues for all player actions
- **Fair difficulty**: Challenging but not frustrating
- **High replayability**: Procedural generation and unlocks

### Technical Constraints
- **Memory**: Limited RAM, be careful with assets
- **Performance**: 20 FPS target, optimize accordingly
- **Storage**: Keep save files small
- **Controls**: Crank + D-pad + 2 buttons only
- **Screen**: 400x240 1-bit display

### Open Questions
- Should there be a hard mode or difficulty settings?
- Should equipment be permanent or temporary per run?
- Should there be a player hub/home base?
- Should multiplayer be explored (local co-op)?
- Should there be a permadeath mode vs continue mode?

---

## Contributing Ideas

Have an idea for CardoBlast? Here's how to contribute:

1. **Open an issue** on GitHub with your suggestion
2. **Tag it** as "enhancement" or "feature-request"
3. **Describe**:
   - What the feature is
   - Why it would improve the game
   - How it could work (optional)
4. **Discuss** with the community

---

## Changelog

### Recent Additions
- ✅ Tactical slowdown ability (2026-01-21)
- ✅ Procedural map generation (2026-01-20)
- ✅ Save/load system (2026-01-15)
- ✅ Level-up system with skill points (2026-01-12)
- ✅ Multiple magic types (2026-01-10)
- ✅ Enemy AI variations (2026-01-08)

### In Progress
- 🔄 Sound and music system
- 🔄 Visual effects polish
- 🔄 Equipment system design

---

**Last Updated**: 2026-01-23
**Maintainer**: Sergio Prada

*This is a living document - priorities and ideas will evolve as development continues!*
