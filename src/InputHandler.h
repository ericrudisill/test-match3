#pragma once

#include <SDL3/SDL.h>

class InputHandler {
public:
    InputHandler(int gemSize, int gridOffsetX, int gridOffsetY);

    void handleEvent(const SDL_Event& event);
    void update(int gemSize, int gridOffsetX, int gridOffsetY);

    bool hasSelection() const { return selectedRow >= 0; }
    void getSelection(int& row, int& col) const { row = selectedRow; col = selectedCol; }
    void clearSelection() { selectedRow = -1; selectedCol = -1; }

    bool hasPendingSwap() const { return pendingSwap; }
    void getSwap(int& row1, int& col1, int& row2, int& col2) const {
        row1 = selectedRow;
        col1 = selectedCol;
        row2 = swapRow;
        col2 = swapCol;
    }
    void clearSwap() { pendingSwap = false; }

private:
    int gemSize;
    int gridOffsetX;
    int gridOffsetY;

    int selectedRow;
    int selectedCol;
    bool pendingSwap;
    int swapRow;
    int swapCol;

    float touchStartX;
    float touchStartY;
    bool isDragging;

    void screenToGrid(float x, float y, int& row, int& col) const;
    bool isValidGridPosition(int row, int col) const;
};
