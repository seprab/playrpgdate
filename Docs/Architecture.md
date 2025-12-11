# Architecture and Design Decisions

This document outlines key architectural decisions and design patterns used in the CardoBlast codebase.

## Memory Management

### Smart Pointers Strategy

The codebase uses modern C++ smart pointers to manage memory safely and avoid leaks:

- **`std::unique_ptr`**: Used for resources with clear single ownership
- **`std::shared_ptr`**: Used for resources with shared ownership
- **Raw pointers**: Used for non-owning references to resources managed elsewhere

### EntityManager Ownership Pattern

**Design Decision**: EntityManager is owned by GameManager as a `std::unique_ptr` and passed to other classes as raw pointers.

**Rationale**:
- **Clear ownership**: GameManager uniquely owns the EntityManager
- **Simple lifetime**: EntityManager is created at game startup and lives for the entire game session
- **Non-owning access**: Classes like Area and UI need to use EntityManager but don't own it
- **Performance**: No reference counting overhead of `shared_ptr`
- **Standard pattern**: Raw pointers for "I don't own this, but I need to use it" is idiomatic C++

**Implementation**:
```cpp
// GameManager owns it
std::unique_ptr<EntityManager> entityManager;

// Pass raw pointer to users
ui = std::make_unique<UI>("fonts/Asheville-Sans-14-Light", entityManager.get());
area->SetEntityManager(entityManager.get());
```

**Why not `shared_ptr`?**
While `shared_ptr` would provide type-safe lifetime guarantees, it would:
- Imply shared ownership when ownership is actually singular
- Add unnecessary reference counting overhead
- Obscure the true ownership model

The raw pointer approach requires discipline to ensure EntityManager outlives its users, but this is naturally guaranteed by the game's initialization order and object lifetimes.

### Animation Resources Lifetime

**Issue**: During the smart pointer refactoring, a critical bug was discovered where `ImageTable` objects were being destroyed while `AnimationClip` objects still held raw pointers to their bitmaps.

**Solution**: Animation resources (ImageTable) must be stored as member variables in the classes that use them, ensuring the resources live as long as the animations need them.

**Example** (Player class):
```cpp
// Player.h
private:
    std::unique_ptr<pdcpp::ImageTable> animationImageTable;
    std::unique_ptr<AnimationClip> idle{};
    std::unique_ptr<AnimationClip> run{};
    // ... other clips

// Player.cpp constructor
animationImageTable = std::make_unique<pdcpp::ImageTable>("images/player/animmini");
auto& anim = *animationImageTable;  // Local reference for convenience

idle->AddBitmap(anim[0]);  // Safe: animationImageTable keeps bitmaps alive
idle->AddBitmap(anim[1]);
```

**Key principle**: If a class stores raw pointers to resources from a smart pointer, that smart pointer must be kept alive for the lifetime of the class.

## Future Considerations

### Magic Classes Player Reference

Currently, Magic classes store a raw `Player*` pointer. This is safe because:
- Magic objects are owned by the Player (`std::vector<std::unique_ptr<Magic>> magicLaunched`)
- Magic objects are destroyed when they expire or when Player is destroyed

However, for extra safety, this could be refactored to use `std::weak_ptr<Player>` in the future.
