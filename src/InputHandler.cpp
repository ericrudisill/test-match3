#include "InputHandler.h"
#include "Grid.h"
#include <cmath>

InputHandler::InputHandler(int gemSize, int gridOffsetX, int gridOffsetY)
    : gemSize(gemSize)
    , gridOffsetX(gridOffsetX)
    , gridOffsetY(gridOffsetY)
    , selectedRow(-1)
    , selectedCol(-1)
    , pendingSwap(false)
    , swapRow(-1)
    , swapCol(-1)
    , touchStartX(0.0f)
    , touchStartY(0.0f)
    , isDragging(false)
{
}

void InputHandler::update(int newGemSize, int newGridOffsetX, int newGridOffsetY) {
    gemSize = newGemSize;
    gridOffsetX = newGridOffsetX;
    gridOffsetY = newGridOffsetY;
}

void InputHandler::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_FINGER_DOWN) {
        float x, y;

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            x = static_cast<float>(event.button.x);
            y = static_cast<float>(event.button.y);
        } else {
            // Finger touch - normalize coordinates
            int windowWidth, windowHeight;
            SDL_Window* window = SDL_GetWindowFromID(event.tfinger.windowID);
            if (window) {
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);
                x = event.tfinger.x * windowWidth;
                y = event.tfinger.y * windowHeight;
            } else {
                return;
            }
        }

        int row, col;
        screenToGrid(x, y, row, col);

        if (isValidGridPosition(row, col)) {
            selectedRow = row;
            selectedCol = col;
            touchStartX = x;
            touchStartY = y;
            isDragging = true;
        }
    }
    else if (event.type == SDL_EVENT_MOUSE_MOTION || event.type == SDL_EVENT_FINGER_MOTION) {
        if (!isDragging || selectedRow < 0) return;

        float x, y;

        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            x = static_cast<float>(event.motion.x);
            y = static_cast<float>(event.motion.y);
        } else {
            int windowWidth, windowHeight;
            SDL_Window* window = SDL_GetWindowFromID(event.tfinger.windowID);
            if (window) {
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);
                x = event.tfinger.x * windowWidth;
                y = event.tfinger.y * windowHeight;
            } else {
                return;
            }
        }

        float dx = x - touchStartX;
        float dy = y - touchStartY;
        float threshold = static_cast<float>(gemSize) * 0.3f;

        if (std::abs(dx) > threshold || std::abs(dy) > threshold) {
            // Determine swap direction
            int targetRow = selectedRow;
            int targetCol = selectedCol;

            if (std::abs(dx) > std::abs(dy)) {
                // Horizontal swipe
                targetCol = (dx > 0) ? selectedCol + 1 : selectedCol - 1;
            } else {
                // Vertical swipe
                targetRow = (dy > 0) ? selectedRow + 1 : selectedRow - 1;
            }

            if (isValidGridPosition(targetRow, targetCol)) {
                swapRow = targetRow;
                swapCol = targetCol;
                pendingSwap = true;
                isDragging = false;
            }
        }
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_FINGER_UP) {
        isDragging = false;
    }
}

void InputHandler::screenToGrid(float x, float y, int& row, int& col) const {
    col = static_cast<int>((x - gridOffsetX) / gemSize);
    row = static_cast<int>((y - gridOffsetY) / gemSize);
}

bool InputHandler::isValidGridPosition(int row, int col) const {
    return row >= 0 && row < Grid::ROWS && col >= 0 && col < Grid::COLS;
}
