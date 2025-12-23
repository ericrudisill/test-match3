#include "Gem.h"
#include <cmath>

Gem::Gem(int row, int col, GemType type)
    : type(type)
    , state(GemState::IDLE)
    , row(row)
    , col(col)
    , targetRow(row)
    , targetCol(col)
    , x(static_cast<float>(col))
    , y(static_cast<float>(row))
    , animationProgress(0.0f)
{
}

void Gem::update(float deltaTime) {
    const float ANIMATION_SPEED = 8.0f;

    if (state == GemState::FALLING || state == GemState::SWAPPING) {
        animationProgress += deltaTime * ANIMATION_SPEED;
        if (animationProgress >= 1.0f) {
            animationProgress = 1.0f;
            row = targetRow;
            col = targetCol;
            x = static_cast<float>(col);
            y = static_cast<float>(row);
            state = GemState::IDLE;
        } else {
            // Smooth interpolation
            float t = animationProgress;
            t = t * t * (3.0f - 2.0f * t); // Smoothstep
            x = col + (targetCol - col) * t;
            y = row + (targetRow - row) * t;
        }
    } else if (state == GemState::EXPLODING) {
        animationProgress += deltaTime * 2.0f;
        if (animationProgress >= 1.0f) {
            state = GemState::MATCHED;
        }
    } else {
        animationProgress = 0.0f;
    }
}

bool Gem::isAnimating() const {
    return state == GemState::FALLING ||
           state == GemState::SWAPPING ||
           state == GemState::EXPLODING;
}
