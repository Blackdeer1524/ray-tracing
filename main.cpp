#include "camera.h"
#include "color.h"
#include "common.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "portal.h"
#include "sphere.h"
#include "vec3.h"

int main() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    auto first_portal =
        make_shared<square_portal>(point3(-4, 3, 5), vec3(-1, 0, 0), 4, vec3(0, 1, 0), 1);
    auto second_portal =
        make_shared<square_portal>(point3(5, 3, 0), vec3(0, 0, 1), 4, vec3(-1, 1, 0), 1);

    auto t = make_shared<lambertian>(color(0.4, 0.2, 0.1));  //

    first_portal->set_fluid(t /* make_shared<portal_fluid>(second_portal) */);
    second_portal->set_fluid(t /* make_shared<portal_fluid>(first_portal) */);

    first_portal->set_fluid(make_shared<portal_fluid>(second_portal));
    second_portal->set_fluid(make_shared<portal_fluid>(first_portal));

    world.add(first_portal);
    world.add(second_portal);

    // Случайным образом разбрасываются сферы
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            point3 center(
                a + 0.9f * random_float(), 0.2, b + 0.9f * random_float());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 500;
    cam.samples_per_pixel = 1;
    cam.max_depth = 13;

    cam.vfov = 40;
    cam.lookfrom = point3(0, 3, -15);
    cam.lookat = point3(0, 0, 1);
    cam.vup = vec3(0, 1, 0);

    cam.focus_dist = 1.0;

    cam.render(world);
}
