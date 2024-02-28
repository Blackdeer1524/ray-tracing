#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

using color = vec3;

inline float linear_to_gamma(float linear_component) {
    return sqrt(linear_component);
}

inline uint32_t write_color(color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    float scale = 1.0f / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity(0.000, 0.999);

    auto ir = static_cast<int>(256 * intensity.clamp(r));
    auto ig = static_cast<int>(256 * intensity.clamp(g));
    auto ib = static_cast<int>(256 * intensity.clamp(b));

    return (ir << 16) + (ig << 8) + ib;
}

#endif
