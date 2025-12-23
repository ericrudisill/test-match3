#include "Renderer.h"
#include "MathUtils.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>
#include <cstdio>

Renderer::Renderer(SDL_Renderer* renderer, int windowWidth, int windowHeight)
    : renderer(renderer)
    , font(nullptr)
    , windowWidth(windowWidth)
    , windowHeight(windowHeight)
    , gemSize(0)
    , gridOffsetX(0)
    , gridOffsetY(0)
    , gemTextures{}
{
    calculateLayout();
    loadGemTextures();

    // Load font from bundled assets directory
    // The font file should be placed in assets/fonts/ relative to the executable
    font = TTF_OpenFont("assets/fonts/DejaVuSans.ttf", 32.0f);

    if (!font) {
        SDL_Log("Warning: Could not load font from assets/fonts/DejaVuSans.ttf: %s", SDL_GetError());
    }
}

void Renderer::loadGemTextures() {
    // Map GemType to sprite file numbers based on colors:
    // RED=06.png, GREEN=02.png, BLUE=01.png, YELLOW=03.png, PURPLE=08.png, ORANGE=04.png
    const char* spriteFiles[] = {
        "assets/sprites/GemStonesV2/64x64px/06.png",  // RED
        "assets/sprites/GemStonesV2/64x64px/02.png",  // GREEN
        "assets/sprites/GemStonesV2/64x64px/01.png",  // BLUE
        "assets/sprites/GemStonesV2/64x64px/03.png",  // YELLOW
        "assets/sprites/GemStonesV2/64x64px/08.png",  // PURPLE
        "assets/sprites/GemStonesV2/64x64px/04.png",  // ORANGE
    };

    for (size_t i = 0; i < static_cast<size_t>(GemType::COUNT); ++i) {
        gemTextures[i] = IMG_LoadTexture(renderer, spriteFiles[i]);
        if (!gemTextures[i]) {
            SDL_Log("Warning: Could not load gem texture %s: %s", spriteFiles[i], SDL_GetError());
        }
    }
}

Renderer::~Renderer() {
    // Clean up gem textures
    for (auto& texture : gemTextures) {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
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

    GemType type = gem->getType();
    if (type == GemType::EMPTY || type == GemType::COUNT) return;

    SDL_FRect rect;
    rect.x = gridOffsetX + gem->getX() * gemSize + 4.0f;
    rect.y = gridOffsetY + gem->getY() * gemSize + 4.0f;
    rect.w = static_cast<float>(gemSize - 8);
    rect.h = static_cast<float>(gemSize - 8);

    SDL_Texture* texture = gemTextures[static_cast<size_t>(type)];
    if (texture) {
        // Draw sprite with alpha modulation
        Uint8 adjustedAlpha = MathUtils::normalizedToByte(alpha);
        SDL_SetTextureAlphaMod(texture, adjustedAlpha);
        SDL_RenderTexture(renderer, texture, nullptr, &rect);
    } else {
        // Fallback to colored rectangle if texture failed to load
        SDL_Color color = getGemColor(type);
        Uint8 adjustedAlpha = MathUtils::normalizedToByte(alpha);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, adjustedAlpha);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Renderer::drawScore(int score) {
    // Draw score background bar
    SDL_FRect scoreBar;
    scoreBar.x = 10;
    scoreBar.y = 10;
    scoreBar.w = static_cast<float>(windowWidth - 20);
    scoreBar.h = 60;

    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &scoreBar);

    if (!font) {
        return;
    }

    // Format score text
    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    // Render text to surface
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, scoreText, 0, textColor);
    if (!textSurface) {
        return;
    }

    // Create texture from surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(textSurface);

    if (!textTexture) {
        return;
    }

    // Get texture dimensions
    float textWidth, textHeight;
    SDL_GetTextureSize(textTexture, &textWidth, &textHeight);

    // Position text centered vertically in the score bar, left-aligned with padding
    SDL_FRect textRect;
    textRect.x = 20;
    textRect.y = scoreBar.y + (scoreBar.h - textHeight) / 2;
    textRect.w = textWidth;
    textRect.h = textHeight;

    SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
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
