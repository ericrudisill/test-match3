#include "Renderer.h"
#include "MathUtils.h"
#include <cmath>

Renderer::Renderer(SDL_Renderer* renderer, int windowWidth, int windowHeight)
    : renderer(renderer)
    , windowWidth(windowWidth)
    , windowHeight(windowHeight)
    , gemSize(0)
    , gridOffsetX(0)
    , gridOffsetY(0)
{
    calculateLayout();
}

Renderer::~Renderer() {
}

void Renderer::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    calculateLayout();
}

void Renderer::calculateLayout() {
    // Calculate gem size based on window dimensions
    int maxGemWidth = windowWidth / Grid::COLS;
    int maxGemHeight = (windowHeight - 100) / Grid::ROWS; // Reserve 100px for UI
    gemSize = std::min(maxGemWidth, maxGemHeight);

    // Center the grid
    int gridWidth = gemSize * Grid::COLS;
    int gridHeight = gemSize * Grid::ROWS;
    gridOffsetX = (windowWidth - gridWidth) / 2;
    gridOffsetY = ((windowHeight - 100) - gridHeight) / 2 + 100; // Offset for score
}

void Renderer::render(const Grid& grid) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
    SDL_RenderClear(renderer);

    drawBackground();
    drawScore(grid.getScore());

    // Draw gems
    for (int row = 0; row < Grid::ROWS; ++row) {
        for (int col = 0; col < Grid::COLS; ++col) {
            const Gem* gem = grid.getGem(row, col);
            if (gem) {
                float alpha = 1.0f;
                if (gem->getState() == GemState::EXPLODING) {
                    // Fade out during explosion
                    alpha = 1.0f - (gem->getY() - gem->getRow());
                    if (alpha < 0.0f) alpha = 0.0f;
                }
                drawGem(gem, alpha);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void Renderer::drawBackground() {
    // Draw grid cells
    for (int row = 0; row < Grid::ROWS; ++row) {
        for (int col = 0; col < Grid::COLS; ++col) {
            SDL_FRect rect;
            rect.x = static_cast<float>(gridOffsetX + col * gemSize + 2);
            rect.y = static_cast<float>(gridOffsetY + row * gemSize + 2);
            rect.w = static_cast<float>(gemSize - 4);
            rect.h = static_cast<float>(gemSize - 4);

            SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void Renderer::drawGem(const Gem* gem, float alpha) {
    if (!gem) return;

    SDL_Color color = getGemColor(gem->getType());
    SDL_FRect rect;
    rect.x = gridOffsetX + gem->getX() * gemSize + 4.0f;
    rect.y = gridOffsetY + gem->getY() * gemSize + 4.0f;
    rect.w = static_cast<float>(gemSize - 8);
    rect.h = static_cast<float>(gemSize - 8);

    Uint8 adjustedAlpha = MathUtils::normalizedToByte(alpha);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, adjustedAlpha);
    SDL_RenderFillRect(renderer, &rect);

    // Draw border (darkened by 30%)
    SDL_SetRenderDrawColor(renderer,
        MathUtils::scaleColorComponent(color.r, 0.7f),
        MathUtils::scaleColorComponent(color.g, 0.7f),
        MathUtils::scaleColorComponent(color.b, 0.7f),
        adjustedAlpha);
    SDL_RenderRect(renderer, &rect);
}

void Renderer::drawScore(int score) {
    // Simple score display using rectangles (no text rendering for simplicity)
    // In a production game, you would use SDL_ttf or similar for text
    SDL_FRect scoreBar;
    scoreBar.x = 10;
    scoreBar.y = 10;
    scoreBar.w = static_cast<float>(windowWidth - 20);
    scoreBar.h = 60;

    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &scoreBar);

    // Draw score indicator (proportional to score)
    SDL_FRect scoreFill;
    scoreFill.x = 15;
    scoreFill.y = 15;
    scoreFill.w = std::min(static_cast<float>(score), scoreBar.w - 10);
    scoreFill.h = 50;

    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    SDL_RenderFillRect(renderer, &scoreFill);
}

SDL_Color Renderer::getGemColor(GemType type) const {
    switch (type) {
        case GemType::RED:    return {220, 50, 50, 255};
        case GemType::GREEN:  return {50, 200, 50, 255};
        case GemType::BLUE:   return {50, 100, 220, 255};
        case GemType::YELLOW: return {230, 230, 50, 255};
        case GemType::PURPLE: return {180, 50, 180, 255};
        case GemType::ORANGE: return {230, 130, 30, 255};
        default:              return {128, 128, 128, 255};
    }
}
