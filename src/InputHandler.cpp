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

void InputHandler::handleEvent(const SDL_Event& event, SDL_Renderer* renderer) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_FINGER_DOWN) {
        float x, y;

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            // Get the scale factor between window and render coordinates
            SDL_Window* window = SDL_GetRenderWindow(renderer);
            int windowWidth, windowHeight, renderWidth, renderHeight;
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            SDL_GetRenderOutputSize(renderer, &renderWidth, &renderHeight);
            float scaleX = static_cast<float>(renderWidth) / windowWidth;
            float scaleY = static_cast<float>(renderHeight) / windowHeight;

            // Scale mouse coordinates to render space
            x = event.button.x * scaleX;
            y = event.button.y * scaleY;
        } else {
            // Finger touch - already normalized to render output size
            int renderWidth, renderHeight;
            SDL_GetRenderOutputSize(renderer, &renderWidth, &renderHeight);
            x = event.tfinger.x * renderWidth;
            y = event.tfinger.y * renderHeight;
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
            // Get the scale factor between window and render coordinates
            SDL_Window* window = SDL_GetRenderWindow(renderer);
            int windowWidth, windowHeight, renderWidth, renderHeight;
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            SDL_GetRenderOutputSize(renderer, &renderWidth, &renderHeight);
            float scaleX = static_cast<float>(renderWidth) / windowWidth;
            float scaleY = static_cast<float>(renderHeight) / windowHeight;

            // Scale mouse coordinates to render space
            x = event.motion.x * scaleX;
            y = event.motion.y * scaleY;
        } else {
            // Finger touch - already normalized to render output size
            int renderWidth, renderHeight;
            SDL_GetRenderOutputSize(renderer, &renderWidth, &renderHeight);
            x = event.tfinger.x * renderWidth;
            y = event.tfinger.y * renderHeight;
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
