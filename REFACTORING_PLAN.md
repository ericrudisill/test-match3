# Game Logic and Rendering Separation Refactoring Plan

## Overview
This plan addresses the coupling between game logic and rendering in the Match-3 game codebase. The goal is to separate concerns so game logic can run independently of visual animations and rendering.

## Current Problems

### 1. Gem Class Mixing Concerns
**Location:** `src/Gem.h:27-46`

The `Gem` class contains both logical state (type, row, col) and visual state (x, y, animation):
```cpp
// Logical properties (good)
GemType type;
int row, col;
GemState state;

// Visual properties (should be separate)
float x, y;                    // Screen positions
float startX, startY;          // Animation start
float animationProgress;       // Animation timing
```

### 2. Grid Manages Animations
**Location:** `src/Grid.cpp:21-29`

Grid (game logic) updates visual animations:
```cpp
void Grid::update(float deltaTime) {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (gems[row][col]) {
                gems[row][col]->update(deltaTime);  // Animation!
            }
        }
    }
}
```

### 3. Game Loop Blocks on Visual State
**Location:** `src/Game.cpp:158-161`

Game logic waits for rendering animations:
```cpp
void Game::updateGameLogic(float deltaTime) {
    if (grid->isAnimating()) {  // Can't proceed while animating
        return;
    }
    // ... game logic ...
}
```

### 4. GemState Enum Mixes Concerns
**Location:** `src/Gem.h:5-12`

Combines semantic states with animation states:
```cpp
enum class GemState {
    IDLE,              // Semantic: ready
    FALLING,           // Animation state
    SWAPPING,          // Animation state
    MATCHED,           // Semantic: matched
    EXPLODING,         // Animation state
    READY_FOR_REMOVAL  // Semantic: cleanup
};
```

### 5. InputHandler Depends on Renderer
**Location:** `src/Game.cpp:60-64`

Input processing requires rendering metrics:
```cpp
inputHandler = std::make_unique<InputHandler>(
    gameRenderer->getGemSize(),      // Renderer dependency
    gameRenderer->getGridOffsetX(),  // Renderer dependency
    gameRenderer->getGridOffsetY()   // Renderer dependency
);
```

### 6. Renderer Accesses Game State Details
**Location:** `src/Renderer.cpp:61-74`

Renderer interprets game logic enums:
```cpp
if (gem->getState() == GemState::EXPLODING) {  // Renderer knows game states
    alpha = 1.0f - (gem->getY() - gem->getRow());
}
```

### 7. Grid Sets Visual Positions
**Location:** `src/Grid.cpp:156-159`

Game logic manipulates screen coordinates:
```cpp
gems[pos.row][pos.col]->setY(-1.0f);  // Grid setting visual position!
gems[pos.row][pos.col]->setTarget(pos.row, pos.col);
```

## Target Architecture

```
┌─────────────────────────────────────────────────────────┐
│ Game (Orchestrator)                                     │
│  - Updates BoardState via BoardLogic                    │
│  - Emits events to GameView                             │
│  - Processes input (logical coordinates only)           │
└────────────┬────────────────────────────┬───────────────┘
             │                            │
             ▼                            ▼
┌────────────────────────┐    ┌──────────────────────────┐
│ Model Layer            │    │ View Layer               │
│                        │    │                          │
│ ┌────────────────┐     │    │ ┌──────────────────┐    │
│ │ BoardState     │     │    │ │ GameView         │    │
│ │ - Pure data    │     │    │ │ - Visual state   │    │
│ │ - No rendering │     │    │ │ - Animations     │    │
│ └────────────────┘     │    │ │ - GemVisuals     │    │
│                        │    │ └──────────────────┘    │
│ ┌────────────────┐     │    │          ▼              │
│ │ BoardLogic     │     │    │ ┌──────────────────┐    │
│ │ - Game rules   │     │    │ │ Renderer         │    │
│ │ - Match logic  │     │    │ │ - Drawing only   │    │
│ │ - Testable     │     │    │ └──────────────────┘    │
│ └────────────────┘     │    │                          │
│                        │    │ ┌──────────────────┐    │
│ ┌────────────────┐     │    │ │ InputHandler     │    │
│ │ Grid           │     │    │ │ - Logical coords │    │
│ │ - Wraps Board  │     │    │ └──────────────────┘    │
│ │ - Emits events │     │    │                          │
│ └────────────────┘     │    │                          │
└────────────────────────┘    └──────────────────────────┘
```

## Refactoring Steps

### Phase 1: Create GameView Class

**New Files:**
- `src/GameView.h`
- `src/GameView.cpp`

**GameView Structure:**
```cpp
class GameView {
public:
    struct GemVisual {
        float x, y;              // Current screen position
        float startX, startY;    // Animation start position
        float targetX, targetY;  // Animation target position
        float animationProgress; // 0.0 to 1.0
        GemType type;
        bool animating;
    };

    GameView(int rows, int cols);

    // Sync from game state
    void syncFromBoardState(const BoardState& state);

    // Handle specific events
    void onGemFalling(int row, int col, GemType type);
    void onGemSwapping(int row1, int col1, int row2, int col2);
    void onGemMatched(int row, int col);

    // Update animations
    void update(float deltaTime);
    bool isAnimating() const;

    // Access for rendering
    const GemVisual* getVisual(int row, int col) const;
    int getRows() const { return rows; }
    int getCols() const { return cols; }

private:
    int rows, cols;
    std::vector<std::vector<std::unique_ptr<GemVisual>>> visuals;

    void startAnimation(int row, int col, float targetX, float targetY);
};
```

**Implementation Notes:**
- `syncFromBoardState()` creates/destroys GemVisuals to match BoardState
- Animation methods set start/target positions and reset progress
- `update()` interpolates positions based on deltaTime
- No game logic, only visual state management

### Phase 2: Simplify Gem Class

**Modify:** `src/Gem.h`, `src/Gem.cpp`

**Remove from Gem:**
```cpp
// DELETE these fields and methods:
float x, y;
float startX, startY;
float animationProgress;
void update(float deltaTime);
void setTarget(int targetRow, int targetCol);
void setX(float newX);
void setY(float newY);
float getX() const;
float getY() const;
```

**Keep in Gem:**
```cpp
// KEEP these (logical state only):
GemType type;
int row, col;
GemState state;  // Will be simplified in Phase 4
```

### Phase 3: Extract Animation from Grid

**Modify:** `src/Grid.h`, `src/Grid.cpp`

**Remove from Grid:**
```cpp
// DELETE:
void update(float deltaTime);
bool isAnimating() const;

// CHANGE these methods to NOT set visual positions:
void fillEmpty() {
    // Remove: gems[pos.row][pos.col]->setY(-1.0f);
    // Remove: gems[pos.row][pos.col]->setTarget(pos.row, pos.col);
    // Keep: syncBoardToGem(), setState() for logical state
}
```

**Add to Grid (optional - for event system):**
```cpp
// Event callback system
std::function<void(int row, int col, GemType type)> onGemFalling;
std::function<void(int row, int col)> onGemMatched;
std::function<void(int r1, int c1, int r2, int c2)> onGemSwapping;

// Call these in appropriate methods
```

### Phase 4: Split GemState Enum

**Modify:** `src/Gem.h`

**Create two separate enums:**
```cpp
// Logical state (in Gem.h)
enum class GemLogicState {
    NORMAL,          // Can be matched/swapped
    MATCHED,         // Matched, will be removed
    REMOVED          // Already removed
};

// Visual state (in GameView.h)
enum class GemAnimState {
    IDLE,
    FALLING,
    SWAPPING,
    EXPLODING
};
```

**Update Gem class:**
```cpp
class Gem {
    GemType type;
    int row, col;
    GemLogicState state;  // Was GemState
};
```

**Update GemVisual:**
```cpp
struct GemVisual {
    // ... existing fields ...
    GemAnimState animState;
};
```

### Phase 5: Update Game Class

**Modify:** `src/Game.cpp`, `src/Game.h`

**Add GameView member:**
```cpp
class Game {
    // ... existing members ...
    std::unique_ptr<GameView> gameView;
};
```

**Initialize in constructor:**
```cpp
Game::Game() {
    // ... existing initialization ...
    gameView = std::make_unique<GameView>(Grid::ROWS, Grid::COLS);

    // Connect Grid events to GameView (if using event system)
    grid->onGemFalling = [this](int row, int col, GemType type) {
        gameView->onGemFalling(row, col, type);
    };
    // ... other event connections ...
}
```

**Update game loop:**
```cpp
void Game::update(float deltaTime) {
    // Update animations (independent of logic)
    gameView->update(deltaTime);

    // Update game logic (no longer blocked by animations!)
    updateGameLogic(deltaTime);
}

void Game::updateGameLogic(float deltaTime) {
    // Remove: if (grid->isAnimating()) return;

    // Game logic can now run at full speed
    switch (state) {
        case GameState::REMOVING_MATCHES:
            grid->removeMatches();
            grid->applyGravity();
            grid->fillEmpty();

            // Sync visual state after logic updates
            gameView->syncFromBoardState(grid->getBoardState());

            // Only wait for animations before accepting input
            if (!gameView->isAnimating()) {
                state = GameState::PLAYING;
            }
            break;
    }
}
```

### Phase 6: Update Renderer

**Modify:** `src/Renderer.cpp`, `src/Renderer.h`

**Change render signature:**
```cpp
// OLD:
void render(const Grid& grid);

// NEW:
void render(const GameView& view);
```

**Update render implementation:**
```cpp
void Renderer::render(const GameView& view) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    drawGrid(view.getRows(), view.getCols());

    for (int row = 0; row < view.getRows(); ++row) {
        for (int col = 0; col < view.getCols(); ++col) {
            const auto* visual = view.getVisual(row, col);
            if (visual) {
                drawGemVisual(visual);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void Renderer::drawGemVisual(const GameView::GemVisual* visual) {
    SDL_FRect rect;
    rect.x = gridOffsetX + visual->x * gemSize + 4.0f;
    rect.y = gridOffsetY + visual->y * gemSize + 4.0f;
    rect.w = gemSize - 8.0f;
    rect.h = gemSize - 8.0f;

    float alpha = 1.0f;
    if (visual->animState == GemAnimState::EXPLODING) {
        alpha = visual->animationProgress;  // Visual state, not game state
    }

    // ... rest of drawing code ...
}
```

**Add coordinate conversion:**
```cpp
// Add to Renderer class
bool Renderer::screenToGrid(float screenX, float screenY,
                            int& row, int& col, int gridRows, int gridCols) const {
    col = static_cast<int>((screenX - gridOffsetX) / gemSize);
    row = static_cast<int>((screenY - gridOffsetY) / gemSize);

    return row >= 0 && row < gridRows && col >= 0 && col < gridCols;
}
```

### Phase 7: Decouple InputHandler

**Modify:** `src/InputHandler.h`, `src/InputHandler.cpp`, `src/Game.cpp`

**Remove Renderer dependencies from InputHandler:**
```cpp
// OLD constructor:
InputHandler(float gemSize, float gridOffsetX, float gridOffsetY);

// NEW constructor:
InputHandler(int gridRows, int gridCols);
```

**Update Game initialization:**
```cpp
// OLD:
inputHandler = std::make_unique<InputHandler>(
    gameRenderer->getGemSize(),
    gameRenderer->getGridOffsetX(),
    gameRenderer->getGridOffsetY()
);

// NEW:
inputHandler = std::make_unique<InputHandler>(Grid::ROWS, Grid::COLS);
```

**Move coordinate conversion to usage site:**
```cpp
void Game::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            float mouseX = event.button.x;
            float mouseY = event.button.y;

            int row, col;
            // Use Renderer to convert screen to grid coords
            if (gameRenderer->screenToGrid(mouseX, mouseY, row, col,
                                          Grid::ROWS, Grid::COLS)) {
                inputHandler->handleClick(row, col);
            }
        }
    }
}
```

## Optional Enhancement: Event System

For cleaner separation, consider adding an event system so Grid doesn't need to know about GameView.

**Create:** `src/BoardEvents.h`
```cpp
enum class BoardEventType {
    GEM_FALLING,
    GEM_SWAPPING,
    GEM_MATCHED,
    GEM_REMOVED
};

struct BoardEvent {
    BoardEventType type;
    int row, col;
    int row2, col2;  // For swapping
    GemType gemType;
};

class BoardEventListener {
public:
    virtual void onBoardEvent(const BoardEvent& event) = 0;
};
```

**Update Grid:**
```cpp
class Grid {
    std::vector<BoardEventListener*> listeners;

    void addListener(BoardEventListener* listener);
    void emitEvent(const BoardEvent& event);
};
```

**Update GameView:**
```cpp
class GameView : public BoardEventListener {
    void onBoardEvent(const BoardEvent& event) override {
        switch (event.type) {
            case BoardEventType::GEM_FALLING:
                onGemFalling(event.row, event.col, event.gemType);
                break;
            // ... handle other events ...
        }
    }
};
```

## Testing Strategy

1. **After Phase 1:** GameView can be tested independently with mock BoardState
2. **After Phase 2-3:** Grid tests should still pass (BoardLogicTests.cpp)
3. **After Phase 6:** Visual tests - verify animations work with new system
4. **After Phase 7:** Input tests - verify coordinate conversion

## Files Modified Summary

### New Files
- `src/GameView.h`
- `src/GameView.cpp`
- `src/BoardEvents.h` (optional)

### Modified Files
- `src/Gem.h` - Remove visual fields, simplify state
- `src/Gem.cpp` - Remove animation methods
- `src/Grid.h` - Remove animation methods, add events
- `src/Grid.cpp` - Remove visual position setting
- `src/Game.h` - Add GameView member
- `src/Game.cpp` - Update loop, remove animation blocking
- `src/Renderer.h` - Change signature, add screenToGrid
- `src/Renderer.cpp` - Render from GameView instead of Grid
- `src/InputHandler.h` - Remove Renderer dependencies
- `src/InputHandler.cpp` - Simplify constructor

### Update Build
- `CMakeLists.txt` - Add GameView.cpp to sources

## Benefits After Refactoring

✅ **Game logic independent of animation speed** - Can run logic at any rate

✅ **Testable without SDL** - Grid/BoardLogic can be tested without graphics

✅ **Visual effects changeable** - Modify animations without touching game code

✅ **Clear boundaries** - Model (Grid/BoardState) → Events → View (GameView) → Renderer

✅ **Better performance potential** - Can update logic and visuals at different rates

✅ **Easier debugging** - Logic and visual bugs are isolated

## Migration Risk Assessment

**Low Risk:**
- Creating GameView (additive change)
- Adding event system (additive change)

**Medium Risk:**
- Updating Renderer to use GameView (signature changes)
- Removing Gem visual fields (breaking change)

**Higher Risk:**
- Splitting GemState enum (many references to update)
- Updating Game loop logic (core game flow)

**Mitigation:**
- Commit after each phase
- Keep game runnable after each phase
- Test thoroughly before proceeding to next phase

## How to Resume

When ready to implement, provide this file to Claude Code and say:

> "Please implement the refactoring plan described in REFACTORING_PLAN.md"

Or start with a specific phase:

> "Please implement Phase 1 of REFACTORING_PLAN.md (Create GameView Class)"
