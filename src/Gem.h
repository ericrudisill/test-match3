#pragma once

#include <SDL3/SDL.h>

enum class GemType {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    ORANGE,
    COUNT
};

enum class GemState {
    IDLE,
    FALLING,
    SWAPPING,
    MATCHED,
    EXPLODING
};

class Gem {
public:
    Gem(int row, int col, GemType type);

    GemType getType() const { return type; }
    GemState getState() const { return state; }
    void setState(GemState newState) { state = newState; }

    int getRow() const { return row; }
    int getCol() const { return col; }
    void setRow(int r) { row = r; }
    void setCol(int c) { col = c; }

    float getX() const { return x; }
    float getY() const { return y; }
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }

    int getTargetRow() const { return targetRow; }
    int getTargetCol() const { return targetCol; }
    void setTarget(int r, int c) { targetRow = r; targetCol = c; }

    void update(float deltaTime);
    bool isAnimating() const;

private:
    GemType type;
    GemState state;
    int row, col;
    int targetRow, targetCol;
    float x, y;
    float animationProgress;
};
