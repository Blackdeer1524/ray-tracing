#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include <cmath>

class camera {
 public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;  // Maximum number of ray bounces into scene
    double vfov = 90;    // in degrees

    point3 lookfrom = point3(0, 0, 0);  // Point camera is looking from
    point3 lookat = point3(0, 0, -1);   // Point camera is looking at
    vec3 vup = vec3(0, 1, 0);           // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist =
        10;  // Distance from camera lookfrom point to plane of perfect focus

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

    vec3 w, u, v;

    vec3 defocus_disk_u;  // Defocus disk horizontal radius
    vec3 defocus_disk_v;  // Defocus disk vertical radius

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        const auto h = std::tan(degrees_to_radians(vfov / 2));
        auto viewport_height = 2 * h * focus_dist;

        const auto viewport_width =
            viewport_height * (static_cast<double>(image_width) / image_height);

        camera_center = lookfrom;
        const point3 viewport_center = camera_center - w * focus_dist;

        const auto v_u = viewport_width * u;
        const auto v_v = -viewport_height * v;

        delta_u = v_u / image_width;
        delta_v = v_v / image_height;

        p00_position =
            viewport_center - v_u / 2 - v_v / 2 + delta_u / 2 + delta_v / 2;

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius =
            focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) {
        // Get a randomly-sampled camera ray for the pixel at location i,j,
        // originating from the camera defocus disk.

        const auto shooting_pos = p00_position + delta_u * j + delta_v * i;
        const auto sampled_shooting_pos = shooting_pos + pixel_sample_suquare();

        auto ray_origin =
            (defocus_angle <= 0) ? camera_center : defocus_disk_sample();

        return ray(camera_center, sampled_shooting_pos - camera_center);
    }

    vec3 pixel_sample_suquare() {
        return delta_u / 2 * random_double() + delta_v / 2 * random_double();
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return camera_center + (p[0] * defocus_disk_u) +
               (p[1] * defocus_disk_v);
    }

    color
    ray_color(const ray &r, const int max_depth, const hittable &world) const {
        if (max_depth <= 0) {
            return color(0, 0, 0);
        }
        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            color attenuation;
            ray scattered;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, max_depth - 1, world);
            }
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif
