#ifndef PORTAL_H
#define PORTAL_H

#include "hittable.h"
#include "vec3.h"
#include <memory>
#include <utility>

class lambertian;

inline vec3 ort(vec3 e1, vec3 e2) {
    if (dot(e1, e2) == 0) {
        return e2;
    }
    const auto a = -e1.length_squared() / dot(e1, e2);
    return e1 + a * e2;
}

class portal : public hittable {
 public:
    portal(point3 center, vec3 q, vec3 p)
        : center_(center), q_(ort(p, q)), p_(p), n_(cross(q, p)) {
        if (dot(q, q_) < 0) {
            q_ = -q_;
        }
        n_ = n_ / n_.length();
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        const auto t_intersection =
            dot(n_, (center_ - r.origin())) / dot(r.direction(), n_);
        if (!ray_t.surrounds(t_intersection)) {
            return false;
        }

        const auto P = r.origin() + t_intersection * r.direction();
        const auto d = P - center_;
        auto ld = d.length();

        const auto q_proj = q_ * dot(q_, d) / q_.length_squared();
        if (q_proj.length() > q_.length()) {
            return false;
        }

        const auto p_proj = p_ * dot(p_, d) / p_.length_squared();
        if (p_proj.length() > p_.length()) {
            return false;
        }

        rec.t = t_intersection;
        rec.mat = fluid_;
        rec.set_face_normal(r, n_);  // поставит флаг

        const auto q_coord = dot(q_proj, q_) > 0
                                 ? q_proj.length() / q_.length()
                                 : -q_proj.length() / q_.length();
        const auto p_coord = dot(p_proj, p_) > 0
                                 ? p_proj.length() / p_.length()
                                 : -p_proj.length() / p_.length();

        // по сути хак как передавать координаты
        rec.p[0] = q_coord;
        rec.p[1] = p_coord;
        rec.p[2] = 0;

        return true;

        // const auto q_proj = q_  *dot
    }

    [[nodiscard]] vec3 get_normal() const {
        return n_;
    }

    [[nodiscard]] vec3 get_p() const {
        return p_;
    }

    [[nodiscard]] vec3 get_q() const {
        return q_;
    }

    [[nodiscard]] point3 get_center() const {
        return center_;
    }

    void set_fluid(std::shared_ptr<material> fluid) {
        fluid_ = fluid;
    }

 private:
    const point3 center_;
    const vec3 p_;
    vec3 q_;
    vec3 n_;
    std::shared_ptr<material> fluid_;
};

#endif
