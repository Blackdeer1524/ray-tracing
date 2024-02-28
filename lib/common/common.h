#ifndef common_H
#define common_H

#include <cmath>
#include <limits>
#include <memory>
#include <numbers>
#include <random>

using std::make_shared;
using std::shared_ptr;
using std::sqrt;
using std::numbers::pi;

const float infinity = std::numeric_limits<float>::max();

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}

inline float random_float() {
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(0, 1);  // range [0, 1)
    return dis(e);
}

inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}

#endif
