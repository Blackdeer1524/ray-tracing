#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <numbers>
#include <random>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;
using std::numbers::pi;

// Constants

const float not_really_infinity = std::numeric_limits<float>::max();

// Utility Functions

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}

inline float random_float() {
    static std::random_device rd;
    static std::ranlux24_base gen(rd());
    static std::uniform_real_distribution<float> uniform_distr(0.0, 1.0);

    return uniform_distr(gen);
}

inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}

// Common Headers

#endif
