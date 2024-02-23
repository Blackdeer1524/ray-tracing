#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
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

const double infinity = std::numeric_limits<double>::infinity();

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> uniform_distr(0.0, 1.0);

    return uniform_distr(gen);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

// Common Headers

#include "ray.h"
#include "vec3.h"

#endif
