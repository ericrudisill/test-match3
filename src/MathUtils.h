#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>
#include <algorithm>

namespace MathUtils {

// Smoothstep interpolation: 3t² - 2t³
// This cubic polynomial has zero derivative at t=0 and t=1,
// creating smooth ease-in-ease-out motion (no abrupt starts/stops).
// Input t should be in range [0, 1].
inline float smoothstep(float t) {
    float tSquared = t * t;
    return (3.0f * tSquared) - (2.0f * tSquared * t);
}

// Linear interpolation between two values
// Returns start when t=0, end when t=1
inline float lerp(float start, float end, float t) {
    return start + (end - start) * t;
}

// Clamp a value to a range [min, max]
template<typename T>
inline T clamp(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

// Clamp delta time to prevent physics/animation breakage from frame spikes
// Default max is 50ms (20 FPS minimum)
inline float clampDeltaTime(float deltaTime, float maxDelta = 0.05f) {
    return std::min(deltaTime, maxDelta);
}

// Scale a color component by a factor (useful for darkening/lightening)
// Result is clamped to [0, 255]
inline unsigned char scaleColorComponent(unsigned char component, float factor) {
    float scaled = static_cast<float>(component) * factor;
    return static_cast<unsigned char>(clamp(scaled, 0.0f, 255.0f));
}

// Convert normalized alpha [0, 1] to byte [0, 255]
inline unsigned char normalizedToByte(float normalized) {
    return static_cast<unsigned char>(clamp(normalized, 0.0f, 1.0f) * 255.0f);
}

// Determine dominant direction from a 2D delta
// Returns: 0 = horizontal dominant, 1 = vertical dominant
inline int dominantDirection(float dx, float dy) {
    return std::abs(dx) > std::abs(dy) ? 0 : 1;
}

// Sign of a value: -1, 0, or 1
template<typename T>
inline int sign(T value) {
    return (T(0) < value) - (value < T(0));
}

} // namespace MathUtils

#endif // MATHUTILS_H
