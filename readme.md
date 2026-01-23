# CardoBlast

A top-down RPG game for the Playdate console where a magician fights against waves of monsters in procedurally generated arenas. Built with C++ using the Playdate SDK.

![Playdate Console](https://play.date/assets/playdate-device.png)

## About the Project

CardoBlast is a fast-paced survival RPG that challenges players to survive as long as possible against increasing waves of monsters. The game features:

- **Procedurally Generated Maps** - Every playthrough offers a unique battlefield
- **Multiple Magic Abilities** - Unlock and switch between different magical attacks
- **RPG Progression** - Level up, allocate skill points, and grow stronger
- **Enemy Variety** - Face different monster types with unique behaviors (melee, ranged, kiting, stationary)
- **Strategic Combat** - Use a slowdown ability to plan your next move
- **Auto-Fire System** - Passive ability that targets nearest enemies
- **Save/Load System** - Continue your progress across sessions

This project was created as a learning exercise for C++ game development and to share something fun with the community.

## Features

### Gameplay Systems
- **Dynamic Combat** - Fast-paced action with multiple magic spells and abilities
- **Character Progression** - Level up system with skill point allocation (Strength, Agility, Constitution, Max HP)
- **Enemy AI** - Multiple enemy types with different movement patterns and attack strategies
- **Procedural Generation** - Unique maps every game with validation for connectivity
- **Time-Attack Mode** - Survive as long as possible and beat your high score
- **Tactical Slowdown** - Special ability that slows enemies when you stop moving (10-second cooldown)

### Technical Features
- **Smart Pointer Memory Management** - Modern C++ with safe memory handling
- **Entity Component System** - Clean separation of game entities
- **A* Pathfinding** - Intelligent enemy navigation
- **JSON-Based Data** - Easy to modify game content
- **Optimized Rendering** - Field-of-view culling for smooth performance
- **Save/Load System** - Persistent game state with JSON serialization

## Controls

### Movement
- **D-Pad** - Move player in 8 directions

### Combat
- **Crank** - Aim your magic spells
- **A Button** - Cycle through available magic spells (forward)
- **B Button** - Cycle through available magic spells (backward)
- **Hold Button** - Auto-fire at nearest enemy (always active)

### Menu
- **Menu Button** - Access system menu (stats, save game)
- **A Button** - Confirm selection in menus
- **D-Pad Up/Down** - Navigate menu options

## Magic Abilities

1. **Beam** - Powerful beam attack (2s cooldown)
2. **Projectile** - Fast traveling projectile (1.5s cooldown)
3. **Orbiting Projectiles** - Protective orbs that circle the player (5s cooldown)
4. **Rapid Shot** - Quick, low-damage projectiles (0.8s cooldown)
5. **Heavy Beam** - Extra powerful beam with extended range (3.5s cooldown)

## Documentation

Detailed documentation is available in the `Docs/` directory:

- **[Architecture](Docs/Architecture.md)** - Architectural decisions and design patterns
- **[Game Systems](Docs/GAME_SYSTEMS.md)** - Detailed explanation of game systems
- **[Building](Docs/Building.md)** - Build instructions for different environments
- **[Maps](Docs/Maps.md)** - Map creation and procedural generation
- **[Save System](Docs/SAVE_SYSTEM_README.md)** - Save/load system documentation
- **[UI Refactoring](Docs/UI_REFACTORING_GUIDE.md)** - UI system implementation guide
- **[Next Steps](Docs/NextSteps.md)** - Planned features and improvements

## Quick Start

### Prerequisites
- Playdate SDK v2.6.2 (required for compatibility with playdate-cpp)
- CMake 3.10 or higher
- C++17 compatible compiler

### Building

1. Set the Playdate SDK environment variable:
```bash
export PLAYDATE_SDK_PATH="/path/to/PlaydateSDK/"
```

2. Run the bootstrap script:
```bash
chmod +x bootstrap.sh
./bootstrap.sh
```

3. Select your build target:
   - Option 1: Build for Simulator
   - Option 2: Build for Device

For detailed build instructions, see [Building.md](Docs/Building.md).

## Project Structure

```
playdate-rpg/
├── src/                    # C++ source files
│   ├── main.cpp           # Entry point
│   ├── GameManager.*      # Main game loop and state management
│   ├── Entity.*           # Base entity class
│   ├── Creature.*         # Base creature class (Player, Monster)
│   ├── Player.*           # Player character implementation
│   ├── Monster.*          # Enemy AI and behavior
│   ├── Area.*             # Level/map management
│   ├── Magic.*            # Magic spell system
│   ├── UI.*               # User interface
│   ├── EntityManager.*    # Entity registry and management
│   ├── SaveGame.*         # Save/load system
│   └── Globals.h          # Game constants and configuration
├── Docs/                  # Documentation
├── data/                  # JSON data files
│   ├── creatures.json     # Monster definitions
│   ├── areas.json         # Area configurations
│   ├── items.json         # Item data
│   ├── weapons.json       # Weapon stats
│   └── armors.json        # Armor stats
└── Source/images/         # Game assets
```

## Code Quality

This project follows modern C++ best practices:
- ✅ Smart pointers for memory management
- ✅ RAII principles
- ✅ Const correctness
- ✅ Consistent formatting and style
- ✅ Named constants (no magic numbers)
- ✅ Comprehensive error handling
- ✅ Clean separation of concerns

## Credits and Libraries

This project wouldn't be possible without these excellent libraries and resources:

### Core Libraries
- **[Playdate SDK](https://play.date/)** - Official Playdate development kit by Panic
- **[playdate-cpp](https://github.com/nstbayless/playdate-cpp)** - C++ wrapper for Playdate API
- **[playdate-cpp-extensions](https://github.com/metaphaseaudio/playdate-cpp-extensions)** - Additional C++ utilities

### Utilities
- **[JSMN](https://github.com/zserge/jsmn)** - Lightweight JSON parser
- **[Flare Engine](https://github.com/flareteam/flare-engine)** - A* pathfinding implementation reference

### Inspiration
- **[UtilForever RPG](https://github.com/utilForever/SimpleRPG-Text)** - RPG system design inspiration
- **[Solarus](https://gitlab.com/solarus-games/solarus)** - RPG engine reference architecture

### Tools
- **[OneBitDitherTool](https://github.com/timheigames/onebitdithertool)** - Sprite conversion tool
- **[Tiled Map Editor](https://www.mapeditor.org/)** - Level design tool

## Contributing

This is a personal learning project, but feedback and suggestions are welcome! Feel free to:
- Report bugs or issues
- Suggest new features
- Share your high scores
- Fork and experiment

## License

This project is open source for learning and reference purposes. Please credit appropriately if you use significant portions of the code.

## Development Status

**Current Version**: Alpha
**Status**: Active Development

### Implemented Features
- ✅ Core gameplay loop
- ✅ Player movement and combat
- ✅ Multiple enemy types with AI
- ✅ Magic spell system
- ✅ Level progression and stats
- ✅ Procedural map generation
- ✅ Save/load system
- ✅ UI system with multiple screens
- ✅ High score tracking

### Planned Features
- 🔲 Story mode and narrative
- 🔲 More enemy types and boss battles
- 🔲 Additional magic abilities
- 🔲 Equipment system
- 🔲 Multiple biomes/environments
- 🔲 Sound effects and music
- 🔲 Achievement system

For a complete roadmap, see [NextSteps.md](Docs/NextSteps.md).

## Contact

Created by Sergio Prada - Building in public and learning C++ game development.

---

**Made with ❤️ for the Playdate community**
