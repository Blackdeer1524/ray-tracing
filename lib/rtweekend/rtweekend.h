#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <numbers>

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

// Common Headers

#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif
