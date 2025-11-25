#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "hittable.h"
#include "material.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>

class camera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;

    double vfov = 90;
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1);
    vec3 vup = vec3(0, 1, 0);

    double defocus_angle = 0;
    double focus_dist = 10;

    void render(const hittable &world)
    {
        initialize();

        std::vector<color> image_buffer(image_height * image_width);

        int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0)
            num_threads = 4;

        std::clog << "Detected " << num_threads << "threads. Starting render...\n";

        std::vector<std::thread> threads;
        int rows_per_thread = image_height / num_threads;

        for (int t = 0; t < num_threads; t++)
        {
            int start_row = t * rows_per_thread;
            int end_row = (t == num_threads - 1) ? image_height : (t + 1) * rows_per_thread;

            threads.emplace_back(&camera::render_worker, this, start_row, end_row, std::ref(world), std::ref(image_buffer));
        }

        for (auto &thread : threads)
        {
            if (thread.joinable())
                thread.join();
        }

        std::cout << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n";

        for (const auto &pixel_color : image_buffer)
        {
            write_color(std::cout, pixel_color);
        }

        std::clog << "\rDone             \n";
    }

private:
    int image_height;
    double pixel_samples_scale;
    point3 center;
    point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u, v, w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;
    vec3 viewport_u;
    vec3 viewport_v;

    void render_worker(int start_row, int end_row, const hittable &world, std::vector<color> &buffer)
    {
        for (int j = start_row; j < end_row; ++j)
        {
            if (j % 10 == 0)
                std::clog << '.' << std::flush;

            for (int i = 0; i < image_width; i++)
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                int index = j * image_width + i;
                buffer[index] = pixel_samples_scale * pixel_color;
            }
        }
    }

    void initialize()
    {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Calculate the u, v, w unit basis vectors for the cmera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewpoint edges.
        viewport_u = viewport_width * u;
        viewport_v = viewport_height * -v;

        // Calculate the horizental and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const
    {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const
    {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const
    {
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray &r, int depth, const hittable &world) const
    {
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec))
        {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};
#endif