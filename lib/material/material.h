#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"
#include "portal.h"
#include "ray.h"

class material {
 public:
    virtual ~material() = default;

    virtual bool scatter(const ray &r_in,
                         const hit_record &rec,
                         color &attenuation,
                         ray &scattered) const = 0;
};

class lambertian : public material {
 public:
    explicit lambertian(const color &a) : albedo(a) {
    }

    bool scatter(const ray &r_in,
                 const hit_record &rec,
                 color &attenuation,
                 ray &scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Отлавливает случай когда нормаль к поверхности и случайное
        // направление диффузного отражения коллапсируют к вектору по норме
        // близким к нулю. Из-за этого могут возникнуть ошибки округления
        // которые могу сделать направление нового луча внутрь объекта (против
        // нормали)
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

 private:
    color albedo;
};

class metal : public material {
 public:
    metal(const color &a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {
    }

    bool scatter(const ray &r_in,
                 const hit_record &rec,
                 color &attenuation,
                 ray &scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_unit_vector());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

 private:
    color albedo;
    float fuzz;
};

class portal_fluid : public material {
 public:
    portal_fluid(std::shared_ptr<square_portal> other) : other_(std::move(other)) {
    }

    bool scatter(const ray &r_in,
                 const hit_record &rec,
                 color &attenuation,
                 ray &scattered) const override {
        const auto diff = other_->get_normal() - rec.normal;

        const auto q_coord = rec.p[0];
        const auto p_coord = rec.p[1];
        const auto loc = other_->get_center() + p_coord * other_->get_p() +
                         q_coord * other_->get_q();

        attenuation = color(1, 1, 1);
        scattered = ray(loc, -unit_vector(r_in.direction()) + diff);
        return true;
    }

 private:
    std::shared_ptr<square_portal> other_;
};

class dielectric : public material {
 public:
    dielectric(float index_of_refraction) : ir(index_of_refraction) {
    }

    bool scatter(const ray &r_in,
                 const hit_record &rec,
                 color &attenuation,
                 ray &scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());

        float cos_theta = std::min(dot(-unit_direction, rec.normal), 1.0f);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        // из Закон Снеллиуса следует, что справа будет стоять синус
        // угла преломления. Синус, очевидно, не может превышать единицу.
        // Если левая часть строго больше единицы, тогда происходит
        // полное внутреннее отражение
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract ||
            reflectance(cos_theta, refraction_ratio) > random_float()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray(rec.p, direction);
        return true;
    }

 private:
    float ir;  // индекс преломления (Index of Refraction)

    static float reflectance(float cosine, float ref_idx) {
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif
