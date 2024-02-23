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

    void render(const hittable &world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (int i = 0; i < image_height; ++i) {
            std::clog << "\rScanlines remaining: " << (image_height - i) << ' '
                      << std::flush;
            for (int j = 0; j < image_width; ++j) {
                const auto shooting_pos =
                    p00_position + vec3(delta_u * j, delta_v * i, 0);
                const ray r(camera_center, shooting_pos - camera_center);
                const auto c = ray_color(r, world);

                write_color(std::cout, c);
            }
        }
        std::clog << "\rDone.                 \n";
    }

 private:
    int image_height;
    point3 camera_center;
    point3 p00_position;
    double delta_u;
    double delta_v;

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

        delta_u = viewport_width / image_width;
        delta_v = -viewport_height / image_height;

        p00_position = viewport_position - v_u / 2 - v_v / 2 +
                       vec3(delta_u / 2, 0, 0) + vec3(0, delta_v / 2, 0);
    }

    color ray_color(const ray &r, const hittable &world) const {
        hit_record rec;

        if (world.hit(r, interval(0, infinity), rec)) {
            return 0.5 * (rec.normal + color(1, 1, 1));
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif
