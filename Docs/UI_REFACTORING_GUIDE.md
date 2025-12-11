# UI Refactoring Guide - Using pdcpp Utilities

This guide documents the refactoring of the CardoBlast UI system to use the pdcpp wrapper library for cleaner, more maintainable code.

## Table of Contents
1. [Why Refactor?](#why-refactor)
2. [pdcpp Utilities Overview](#pdcpp-utilities-overview)
3. [Refactoring Patterns](#refactoring-patterns)
4. [Screen-by-Screen Changes](#screen-by-screen-changes)
5. [Best Practices](#best-practices)

---

## Why Refactor?

### Before:
```cpp
// Manual coordinate calculations
int textWidth = GetTextWidth(text);
int x = UIConstants::SCREEN_CENTER_X - (textWidth / 2);
pd->graphics->drawText(text, strlen(text), kASCIIEncoding, x, y);

// Magic numbers and manual rectangle creation
pd->graphics->fillRect(80, 30, 240, 160, kColorBlack);
```

### After:
```cpp
// Declarative, self-documenting layout
auto panel = screenBounds.removeFromTop(190).reduced(80, 30);
pdcpp::Graphics::fillRoundedRectangle(panel, 8, pdcpp::Colors::black);

// Built-in text alignment
font.drawWrappedText(text, bounds, pdcpp::Font::Center, pdcpp::Font::Middle);
```

### Benefits:
- ✅ **Less manual math** - Rectangle utilities handle positioning
- ✅ **More readable** - Code expresses intent clearly
- ✅ **Easier to modify** - Change layouts without recalculating coordinates
- ✅ **Type-safe** - C++ types instead of raw API calls
- ✅ **Prettier visuals** - Rounded corners, proper alignment, consistent spacing

---

## pdcpp Utilities Overview

### 1. Rectangle Layout System

The `pdcpp::Rectangle<T>` class provides powerful layout utilities:

#### Slicing Methods (removeFrom)
```cpp
auto screenBounds = pdcpp::Graphics::getScreenBounds();

// Remove slices from edges - original rect is modified!
auto header = screenBounds.removeFromTop(40);      // Takes top 40px
auto footer = screenBounds.removeFromBottom(30);   // Takes bottom 30px
auto leftPanel = screenBounds.removeFromLeft(100); // Takes left 100px
auto rightPanel = screenBounds.removeFromRight(80); // Takes right 80px

// screenBounds is now the remaining center area
```

**Visual Example:**
```
Before:                    After removeFromTop(40):
┌──────────────────┐       ┌──────────────────┐ ← header (40px)
│                  │       ├──────────────────┤
│   screenBounds   │       │                  │
│                  │       │   screenBounds   │ ← modified
│                  │       │   (remaining)    │
└──────────────────┘       └──────────────────┘
```

#### Sizing Methods
```cpp
auto outer = Rectangle<int>(0, 0, 100, 100);

// Inset on all sides
auto inner = outer.reduced(10);           // {10, 10, 80, 80}

// Expand on all sides
auto expanded = outer.expanded(5);        // {-5, -5, 110, 110}

// Change dimensions
auto wider = outer.withWidth(150);        // {0, 0, 150, 100}
auto taller = outer.withHeight(150);      // {0, 0, 100, 150}
```

#### Position Getters/Setters
```cpp
Rectangle<int> rect(50, 50, 100, 80);

// Get positions
auto center = rect.getCenter();           // {100, 90}
auto topLeft = rect.getTopLeft();         // {50, 50}
auto bottomRight = rect.getBottomRight(); // {150, 130}

// Set positions
rect.setCenter(Point<int>(200, 120));     // Moves rect to center on this point
rect.alignLeft(0);                        // Aligns left edge to x=0
rect.alignTop(0);                         // Aligns top edge to y=0
```

### 2. Font and Text System

The `pdcpp::Font` class wraps LCDFont with C++ conveniences:

#### Basic Usage
```cpp
// Load font (can be stored as member variable)
pdcpp::Font font("/System/Fonts/Asheville-Sans-14-Bold.pft");

// Simple text drawing
font.drawText("Hello World", 10, 20);

// Measure text
int width = font.getTextWidth("Some text");
Rectangle<int> bounds = font.getTextArea("Some text");
int height = font.getFontHeight();
```

#### Text Wrapping and Justification
```cpp
// Automatic word wrapping with alignment
font.drawWrappedText(
    "This is a long text that will automatically wrap to fit the bounds",
    Rectangle<float>(10, 10, 200, 100),
    pdcpp::Font::Center,   // Horizontal: Left, Center, Right
    pdcpp::Font::Middle    // Vertical: Top, Middle, Bottom
);

// Get wrapped text as array of strings (useful for custom rendering)
std::vector<std::string> lines = font.wrapText("Long text...", 200);
```

### 3. Graphics Drawing Methods

#### Rounded Rectangles (NEW!)
```cpp
// Filled rounded rectangle
pdcpp::Graphics::fillRoundedRectangle(
    Rectangle<int>(10, 10, 100, 50),
    8,                      // Corner radius in pixels
    pdcpp::Colors::black
);

// Outlined rounded rectangle
pdcpp::Graphics::drawRoundedRectangle(
    Rectangle<int>(10, 10, 100, 50),
    8,                      // Corner radius
    2,                      // Line thickness
    pdcpp::Colors::white
);
```

#### Basic Shapes (Already Using)
```cpp
// Fill rectangle
pdcpp::Graphics::fillRectangle(rect, pdcpp::Colors::black);

// Draw rectangle outline
pdcpp::Graphics::drawRectangle(rect, pdcpp::Colors::white);

// Ellipses
pdcpp::Graphics::fillEllipse(rect, 0, 360, pdcpp::Colors::white);
pdcpp::Graphics::drawEllipse(rect, 2, 0, 360, pdcpp::Colors::white);
```

### 4. Color Patterns

Available in `pdcpp::Colors`:

```cpp
// Solid colors
pdcpp::Colors::black
pdcpp::Colors::white
pdcpp::Colors::clear

// 50% transparency effects
pdcpp::Colors::transparent50GrayA  // Checkerboard pattern A
pdcpp::Colors::transparent50GrayB  // Checkerboard pattern B (what you're using!)

// Gradient-like dithering (0 = lightest, 6 = darkest)
pdcpp::Colors::steppedDither0   // ~10% black
pdcpp::Colors::steppedDither1   // ~20% black
pdcpp::Colors::steppedDither2   // ~35% black
pdcpp::Colors::steppedDither3   // ~50% black (medium gray)
pdcpp::Colors::steppedDither4   // ~65% black
pdcpp::Colors::steppedDither5   // ~80% black
pdcpp::Colors::steppedDither6   // ~90% black

// Pattern fills
pdcpp::Colors::sparseCheckerA
pdcpp::Colors::diagonalLinesRightWhiteOnBlack
pdcpp::Colors::verticalLines25A

// Gradient array (9 levels from black to white)
pdcpp::Colors::gradient[0]  // Black
pdcpp::Colors::gradient[4]  // Middle gray
pdcpp::Colors::gradient[8]  // White
```

### 5. Border System

The `pdcpp::Border<T>` class manages padding/margins:

```cpp
// Define border/padding (top, bottom, left, right)
pdcpp::Border<int> padding{10, 10, 15, 15};

// Apply padding to get inner bounds
Rectangle<int> outer(0, 0, 100, 100);
Rectangle<int> inner = padding.subtractFrom(outer);
// inner = {15, 10, 70, 80}  (x, y, width, height)
```

### 6. TextComponent (Advanced)

Pre-built component for styled text rendering:

```cpp
// Create text component
pdcpp::TextComponent title(
    "Card oBlast",
    pdcpp::Font::Center,           // Horizontal alignment
    pdcpp::Font::Middle            // Vertical alignment
);

// Set bounds
title.setBounds(Rectangle<float>(0, 0, 400, 60));

// Customize colors
title.setColor(TextComponent::textColorId, pdcpp::Colors::white);
title.setColor(TextComponent::backgroundColorId, pdcpp::Colors::black);

// Set padding
title.setBorderSize(pdcpp::Border<int>{5, 5, 10, 10});

// Draw
title.redraw();
```

---

## Refactoring Patterns

### Pattern 1: Replacing Manual Centering

#### Before:
```cpp
void DrawCenteredText(const char* text, int y, LCDColor color) const {
    int textWidth = GetTextWidth(text);
    int x = UIConstants::SCREEN_CENTER_X - (textWidth / 2);
    SetTextDrawMode(color);
    pd->graphics->drawText(text, strlen(text), kASCIIEncoding, x, y);
    pd->graphics->setDrawMode(kDrawModeCopy);
}

DrawCenteredText("Game Over", 100, kColorWhite);
```

#### After:
```cpp
pdcpp::Font font("/System/Fonts/Asheville-Sans-14-Bold.pft");

// Single line centered at specific Y
Rectangle<float> bounds(0, 100, SCREEN_WIDTH, font.getFontHeight());
font.drawWrappedText("Game Over", bounds, pdcpp::Font::Center, pdcpp::Font::Top);

// Or center both horizontally and vertically in an area
Rectangle<float> area(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
font.drawWrappedText("Game Over", area, pdcpp::Font::Center, pdcpp::Font::Middle);
```

### Pattern 2: Layout with Rectangle Slicing

#### Before:
```cpp
// Manual coordinate calculation
pdcpp::Rectangle<int> banner(0, 170, 400, 100);
pdcpp::Rectangle<int> progressBar(50, 215, 300, 10);
```

#### After:
```cpp
auto screen = pdcpp::Graphics::getScreenBounds();
auto banner = screen.removeFromBottom(100);
auto progressArea = banner.reduced(50, 0);  // 50px horizontal margin
auto progressBar = progressArea.removeFromBottom(10);
progressBar.alignBottom(215);
```

### Pattern 3: Rounded Panels for Polish

#### Before:
```cpp
// Sharp corners
pdcpp::Graphics::fillRectangle(panel, pdcpp::Colors::black);
```

#### After:
```cpp
// Rounded corners look much nicer!
pdcpp::Graphics::fillRoundedRectangle(panel, 8, pdcpp::Colors::black);

// Optional: Add a border
pdcpp::Graphics::drawRoundedRectangle(panel, 8, 2, pdcpp::Colors::white);
```

### Pattern 4: Multi-line Text with Word Wrapping

#### Before:
```cpp
// Manual line management
const char* line1 = "Game Over";
const char* line2 = "Press A to return to the main menu.";

int line1Width = GetTextWidth(line1);
int x1 = offset.x - (line1Width / 2);
int y1 = offset.y - 16;
pd->graphics->drawText(line1, strlen(line1), kASCIIEncoding, x1, y1);

int line2Width = GetTextWidth(line2);
int x2 = offset.x - (line2Width / 2);
int y2 = offset.y + 16;
pd->graphics->drawText(line2, strlen(line2), kASCIIEncoding, x2, y2);
```

#### After:
```cpp
// Automatic wrapping and centering
std::string text = "Game Over\n\nPress A to return to the main menu.";
Rectangle<float> textArea(
    offset.x - 200, offset.y - 60,
    400, 120
);

font.drawWrappedText(
    text,
    textArea,
    pdcpp::Font::Center,  // Horizontally centered
    pdcpp::Font::Middle   // Vertically centered
);
```

---

## Screen-by-Screen Changes

### Loading Screen Refactoring

#### Changes Made:
1. ✅ Replaced `LCDFont*` with `pdcpp::Font` wrapper
2. ✅ Used Rectangle slicing for layout
3. ✅ Used `font.drawWrappedText()` for centered text
4. ✅ Simplified banner and progress bar positioning

#### Key Improvements:
```cpp
// OLD: Manual positioning
pdcpp::Rectangle<int> banner(0, 170, 400, 100);
pdcpp::Rectangle<int> progressBar(50, 215, 300, 10);

// NEW: Declarative layout
auto screen = pdcpp::Graphics::getScreenBounds();
auto banner = screen.removeFromBottom(100);
banner.alignTop(Loading::BANNER_Y);
auto progressArea = banner.reduced(Loading::PROGRESS_BAR_X, 0);
auto progressBar = progressArea.removeFromBottom(Loading::PROGRESS_BAR_HEIGHT);
progressBar.alignTop(Loading::PROGRESS_BAR_Y);
```

### Main Menu Refactoring

#### Changes Made:
1. ✅ Added rounded corners to panel
2. ✅ Used `font.drawWrappedText()` for title
3. ✅ Added rounded corners to menu item selection
4. ✅ Better spacing with Rectangle utilities

#### Key Improvements:
```cpp
// OLD: Sharp corners
pdcpp::Graphics::fillRectangle(panel, pdcpp::Colors::black);

// NEW: Rounded corners (8px radius)
pdcpp::Graphics::fillRoundedRectangle(panel, 8, pdcpp::Colors::black);

// OLD: Manual text centering
DrawCenteredText(title, MainMenu::TITLE_Y, Theme::TEXT_COLOR);

// NEW: Built-in centering
auto titleBounds = Rectangle<float>(0, MainMenu::TITLE_Y, SCREEN_WIDTH, 20);
font.drawWrappedText(title, titleBounds, pdcpp::Font::Center, pdcpp::Font::Top);
```

### Game Over Screen Refactoring

#### Changes Made:
1. ✅ Used single `drawWrappedText()` call for all text
2. ✅ Automatic line breaks with `\n\n`
3. ✅ Simplified from 20+ lines to ~10 lines
4. ✅ Added rounded corners to panel

#### Key Improvements:
```cpp
// OLD: Two separate text draws with manual centering
int gameOverTextWidth = GetTextWidth(gameOverText);
int gameOverX = offset.x - (gameOverTextWidth / 2);
// ... repeat for second line ...

// NEW: One call, automatic centering and wrapping
std::string text = "Game Over\n\nPress A to return to the main menu.";
font.drawWrappedText(
    text,
    panelRect.toFloat(),
    pdcpp::Font::Center,
    pdcpp::Font::Middle
);
```

---

## Best Practices

### 1. Store Font as Member Variable
```cpp
// In UI.h
private:
    pdcpp::Font uiFont;

// In UI constructor
UI::UI(const char *fontPath, EntityManager* manager)
    : uiFont(fontPath)
    , entityManager(manager)
{
    // Font is loaded once and reused
}
```

### 2. Use Rectangle Slicing for Layouts
```cpp
// Good: Declarative, easy to modify
auto screen = pdcpp::Graphics::getScreenBounds();
auto header = screen.removeFromTop(40);
auto footer = screen.removeFromBottom(30);
auto contentArea = screen.reduced(10);  // 10px margin

// Bad: Manual calculations, hard to modify
Rectangle<int> header(0, 0, 400, 40);
Rectangle<int> footer(0, 210, 400, 30);
Rectangle<int> contentArea(10, 50, 380, 150);
```

### 3. Use Rounded Rectangles for Polish
```cpp
// Rounded corners look more professional
pdcpp::Graphics::fillRoundedRectangle(panel, 8, color);

// Combine fill and outline for depth
pdcpp::Graphics::fillRoundedRectangle(panel, 8, pdcpp::Colors::black);
pdcpp::Graphics::drawRoundedRectangle(panel, 8, 2, pdcpp::Colors::white);
```

### 4. Prefer Font::drawWrappedText Over Manual Drawing
```cpp
// Good: Handles alignment and wrapping
font.drawWrappedText(text, bounds, pdcpp::Font::Center, pdcpp::Font::Middle);

// Bad: Manual centering and line breaks
int width = font.getTextWidth(text);
int x = centerX - width / 2;
font.drawText(text, x, y);
```

### 5. Use Named Constants (Already Doing!)
```cpp
// Good: Self-documenting
auto banner = screen.removeFromTop(Loading::BANNER_HEIGHT);

// Bad: Magic numbers
auto banner = screen.removeFromTop(100);
```

### 6. Use Appropriate Dither Patterns
```cpp
// For semi-transparent overlays
pdcpp::Colors::transparent50GrayB

// For gradient effects (darker to lighter)
pdcpp::Colors::steppedDither6  // Darkest
pdcpp::Colors::steppedDither3  // Medium
pdcpp::Colors::steppedDither0  // Lightest

// For pattern fills
pdcpp::Colors::diagonalLinesRightWhiteOnBlack
pdcpp::Colors::verticalLines25A
```

---

## Common Patterns Cheat Sheet

### Centering Text
```cpp
// Horizontal center at specific Y
Rectangle<float> bounds(0, y, SCREEN_WIDTH, font.getFontHeight());
font.drawWrappedText(text, bounds, pdcpp::Font::Center, pdcpp::Font::Top);

// Center both ways in area
font.drawWrappedText(text, area, pdcpp::Font::Center, pdcpp::Font::Middle);
```

### Creating Margins/Padding
```cpp
// All sides equal
auto inner = outer.reduced(10);

// Horizontal and vertical
auto inner = outer.reduced(horizontalMargin, verticalMargin);

// Using Border for different edges
pdcpp::Border<int> border{top, bottom, left, right};
auto inner = border.subtractFrom(outer);
```

### Rounded Panel with Border
```cpp
// Fill with rounded corners
pdcpp::Graphics::fillRoundedRectangle(panel, 8, pdcpp::Colors::black);

// Add outline
pdcpp::Graphics::drawRoundedRectangle(panel, 8, 2, pdcpp::Colors::white);
```

### Layout Slicing
```cpp
auto screen = pdcpp::Graphics::getScreenBounds();

// Take slices from edges
auto topBar = screen.removeFromTop(50);
auto bottomBar = screen.removeFromBottom(30);
auto leftPanel = screen.removeFromLeft(100);

// screen is now the remaining center area
```

### Multi-line Centered Text
```cpp
std::string text = "Line 1\n\nLine 2\n\nLine 3";
font.drawWrappedText(text, bounds, pdcpp::Font::Center, pdcpp::Font::Middle);
```

---

## Performance Notes

1. **Font Loading**: Load fonts once in constructor, not per frame
2. **Rectangle Calculations**: Very fast, no performance concern
3. **Rounded Rectangles**: Slightly slower than regular rectangles, but negligible for UI
4. **Text Wrapping**: Calculation happens each call, consider caching wrapped text if static
5. **Drawing Order**: Draw back-to-front (background → panels → text)

---

## Further Reading

- `playdate-cpp-extensions/inc/pdcpp/graphics/Graphics.h` - All drawing methods
- `playdate-cpp-extensions/inc/pdcpp/graphics/Rectangle.h` - Layout utilities
- `playdate-cpp-extensions/inc/pdcpp/graphics/Font.h` - Text rendering
- `playdate-cpp-extensions/inc/pdcpp/graphics/Colors.h` - Color patterns
- `playdate-cpp-extensions/inc/pdcpp/components/Component.h` - Component system

---

## Migration Checklist

- [x] Create UIConstants.h with layout constants
- [x] Replace pd->graphics->fillRect with pdcpp::Graphics::fillRectangle
- [x] Replace LCDFont* with pdcpp::Font
- [x] Use Rectangle slicing for layouts
- [x] Use Font::drawWrappedText for text alignment
- [x] Add rounded corners to panels
- [x] Use dither patterns for transparency effects
- [ ] Consider TextComponent for complex text
- [ ] Consider ListMenuComponent for menus
- [ ] Add Border utilities for consistent padding

---

**Last Updated**: 2025-12-11
**Author**: Claude Code Refactoring Assistant
