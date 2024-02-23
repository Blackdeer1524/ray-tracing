#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include "vec3.h"

class camera {
 public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;  // Maximum number of ray bounces into scene

    void render(const hittable &world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (int i = 0; i < image_height; ++i) {
            std::clog << "\rScanlines remaining: " << (image_height - i) << ' '
                      << std::flush;
            for (int j = 0; j < image_width; ++j) {
                color c;
                for (int k = 0; k < samples_per_pixel; ++k) {
                    const auto r = get_ray(i, j);
                    c += ray_color(r, max_depth, world);
                }
                write_color(std::cout, c, samples_per_pixel);
            }
        }
        std::clog << "\rDone.                 \n";
    }

 private:
    int image_height;
    point3 camera_center;
    point3 p00_position;
    vec3 delta_u;
    vec3 delta_v;

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        constexpr double focal_length = 1.0;
        constexpr double viewport_height = 2.0;
        const auto viewport_width =
            viewport_height * (static_cast<double>(image_width) / image_height);

        camera_center = point3(0, 0, 0);
        point3 viewport_position = camera_center + vec3(0, 0, -focal_length);

        const auto v_u = vec3(viewport_width, 0, 0);
        const auto v_v = vec3(0, -viewport_height, 0);

        delta_u = vec3(viewport_width / image_width, 0, 0);
        delta_v = vec3(0, -viewport_height / image_height, 0);

        p00_position =
            viewport_position - v_u / 2 - v_v / 2 + delta_u / 2 + delta_v / 2;
    }

    ray get_ray(int i, int j) {
        const auto shooting_pos = p00_position + delta_u * j + delta_v * i;
        const auto sampled_shooting_pos = shooting_pos + pixel_sample_suquare();

        return ray(camera_center, sampled_shooting_pos - camera_center);
    }

    vec3 pixel_sample_suquare() {
        return delta_u / 2 * random_double() + delta_v / 2 * random_double();
    }

    color
    ray_color(const ray &r, const int max_depth, const hittable &world) const {
        if (max_depth <= 0) {
            return color(0, 0, 0);
        }
        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            vec3 direction = random_on_hemisphere(rec.normal);
            return 0.5 * ray_color(ray(rec.p, direction), max_depth - 1, world);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif
