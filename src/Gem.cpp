#include "Gem.h"
#include "MathUtils.h"
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
    , startX(static_cast<float>(col))
    , startY(static_cast<float>(row))
    , animationProgress(0.0f)
{
}

void Gem::update(float deltaTime) {
    const float ANIMATION_SPEED = 2.0f;  // ~0.5 second fall time

    if (state == GemState::FALLING || state == GemState::SWAPPING) {
        // Initialize starting positions on first frame of animation
        if (animationProgress == 0.0f) {
            startX = x;
            startY = y;
        }

        animationProgress += deltaTime * ANIMATION_SPEED;
        if (animationProgress >= 1.0f) {
            animationProgress = 1.0f;
            row = targetRow;
            col = targetCol;
            x = static_cast<float>(col);
            y = static_cast<float>(row);
            state = GemState::IDLE;
        } else {
            float smoothT = MathUtils::smoothstep(animationProgress);
            x = MathUtils::lerp(startX, static_cast<float>(targetCol), smoothT);
            y = MathUtils::lerp(startY, static_cast<float>(targetRow), smoothT);
        }
    } else if (state == GemState::EXPLODING) {
        animationProgress += deltaTime * 2.0f;
        if (animationProgress >= 1.0f) {
            state = GemState::READY_FOR_REMOVAL;
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
