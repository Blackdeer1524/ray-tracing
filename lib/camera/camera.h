#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

class camera {
 public:
    float aspect_ratio = 1.0;  // соотношение сторон результирующего изображения
    int image_width = 100;  // параметр ширины результирующего изображения
    int samples_per_pixel = 10;  // количество лучей, посылаемых в один пиксель.
                                 // Результат усреднится

    int max_depth = 10;  // максимальное количество отскоков луча от объектов
    float vfov = 90;  // задающий вертикальный угол обзора

    point3 lookfrom = point3(0, 0, 0);  // точка откуда смотрит камера
    point3 lookat = point3(0, 0, -1);  // точка куда смотрит камера
    vec3 vup = vec3(0, 1, 0);  // задает где находится верх камеры

    float focus_dist = 10;  // расстояние от камеры до холста

    void render(const hittable &world) {
        initialize();
        std::vector<int> buffer(size_t(image_width) * image_height);

        std::vector<std::thread> pool;
        const auto thread_count = 16;
        for (int k = 1; k < thread_count; k += 1) {
            pool.emplace_back([k, this, &world, &buffer]() {
                for (int i = k; i < image_height; i += thread_count) {
                    for (int j = 0; j < image_width; ++j) {
                        color c;
                        // выпускается samples_per_pixel лучей для получения
                        // информации о пикселе в i-ой строке j-ом столбце.
                        // После цикла функция write_color поделит полученное
                        // значение на количество отправленных лучей
                        for (int k = 0; k < samples_per_pixel; ++k) {
                            const auto r = get_ray(i, j);
                            c += ray_color(r, max_depth, world);
                        }
                        buffer[i * image_width + j] =
                            write_color(c, samples_per_pixel);
                    }
                }
            });
        }

        // Отрисовка в файл
        for (int i = 0; i < image_height; i += thread_count) {
            std::clog << "\rScanlines remaining: " << (image_height - i) << ' '
                      << std::flush;
            for (int j = 0; j < image_width; ++j) {
                color c;
                for (int k = 0; k < samples_per_pixel; ++k) {
                    const auto r = get_ray(i, j);
                    c += ray_color(r, max_depth, world);
                }
                buffer[i * image_width + j] = write_color(c, samples_per_pixel);
            }
        }
        std::clog << "\rDone.                 \n";
        for (auto &th : pool) {
            th.join();
        }

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (auto rgb : buffer) {
            auto b = rgb % 256;
            rgb >>= 8;
            auto g = rgb % 256;
            rgb >>= 8;
            auto r = rgb % 256;
            std::cout << r << ' ' << g << ' ' << b << '\n';
        }
    }

 private:
    int image_height;
    point3 camera_center;
    point3 p00_position;  // позиция верхнего левого пикселя холста в сцене
    vec3 delta_u;
    vec3 delta_v;

    // За деталями см. рисунок 10
    vec3 w, u, v;  // Базисные векторы камеры

    // w - направление взгляда
    // v - направление верха
    // u - направление права

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // за деталями см. Рисунок 11
        const auto h = std::tan(degrees_to_radians(vfov / 2)) * focus_dist;
        // Вычисление высоты холста
        auto viewport_height = 2 * h;

        // Вычисление ширины холста
        const auto viewport_width =
            viewport_height * (static_cast<float>(image_width) / image_height);

        camera_center = lookfrom;
        const point3 viewport_center =
            camera_center - w * focus_dist;  // вычисление точки в пространстве,
                                             // соответствующей центру холста

        // опорный вектор по ширине холста
        const auto v_u = viewport_width * u;
        // опорный вектор по высоте холста
        const auto v_v = -viewport_height * v;

        delta_u = v_u / image_width;
        delta_v = v_v / image_height;

        p00_position =
            viewport_center - v_u / 2 - v_v / 2 + delta_u / 2 + delta_v / 2;
    }

    // Генерирует луч, пускаемый в холст для получения информации о цвете
    // пикселя, находящегося в i-ой строке в j-ом столбце. К направлению
    // луча подмешивается шум
    ray get_ray(int i, int j) {
        const auto shooting_pos = p00_position + delta_u * j + delta_v * i;
        const auto sampled_shooting_pos = shooting_pos + pixel_sample_suquare();
        return ray(camera_center, sampled_shooting_pos - camera_center);
    }

    // генерация случайное отклонение для отправляемого луча
    vec3 pixel_sample_suquare() {
        return delta_u / 2 * random_float() + delta_v / 2 * random_float();
    }

    // отображает объект world на экране
    [[nodiscard]] color
    ray_color(ray r, const int max_depth, const hittable &world) const {
        color cumulative_attenuation(1.0, 1.0, 1.0);
        int i = 0;
        for (; i < max_depth; ++i) {
            hit_record rec;
            if (!world.hit(r, interval(0.001, infinity), rec)) {
                vec3 unit_direction = unit_vector(r.direction());
                float a = 0.5f * (unit_direction.y() + 1.0f);
                return cumulative_attenuation *
                       ((1.0f - a) * color(1.0, 1.0, 1.0) +
                        a * color(0.5, 0.7, 1.0));
            }
            color attenuation;
            ray scattered;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                cumulative_attenuation = cumulative_attenuation * attenuation;
                r = scattered;
            } else if (!attenuation.near_zero()) {
                return cumulative_attenuation * attenuation;
            } else {
                return color(0, 0, 0);
            }
        }
        return color(0, 0, 0);
    }
};

#endif
