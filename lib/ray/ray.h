#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
 public:
    ray() {
    }

    ray(const point3 &origin, const vec3 &direction)
        : orig(origin), dir(direction) {
    }

    [[nodiscard]] point3 origin() const {
        return orig;
    }

    [[nodiscard]] vec3 direction() const {
        return dir;
    }

    [[nodiscard]] point3 at(float t) const {
        return orig + t * dir;
    }

 private:
    point3 orig;
    vec3 dir;
};

#endif
