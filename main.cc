#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "aabb.h"
#include "bvh.h" 
#include <chrono>

int main()
{
    // 1. 像往常一样创建场景，往 world 里塞球
    hittable_list world;

    auto ground_material = make_unique<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_unique<sphere>(point3(0, -1000, 0), 1000, std::move(ground_material)));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                unique_ptr<material> sphere_material;

                if (choose_mat < 0.8)
                {
                    auto albedo = color::random() * color::random();
                    sphere_material = make_unique<lambertian>(albedo);
                    world.add(make_unique<sphere>(center, 0.2, std::move(sphere_material)));
                }
                else if (choose_mat < 0.95)
                {
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_unique<metal>(albedo, fuzz);
                    world.add(make_unique<sphere>(center, 0.2, std::move(sphere_material)));
                }
                else
                {
                    sphere_material = make_unique<dielectric>(1.5);
                    world.add(make_unique<sphere>(center, 0.2, std::move(sphere_material)));
                }
            }
        }
    }

    auto material1 = make_unique<dielectric>(1.5);
    world.add(make_unique<sphere>(point3(0, 1, 0), 1.0, std::move(material1)));

    auto material2 = make_unique<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_unique<sphere>(point3(-4, 1, 0), 1.0, std::move(material2)));

    auto material3 = make_unique<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_unique<sphere>(point3(4, 1, 0), 1.0, std::move(material3)));

    // --- 相机设置保持不变 ---
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;
    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);
    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    std::clog << "Building BVH structure...\n";
    
    hittable_list world_bvh;

    // 使用 bvh_node 的构造函数，把原来的 world 列表的所有权全部“吃掉”
    // 构建完这一行，world 里面就空了，所有的球都按空间顺序整理进了树里
    world_bvh.add(make_unique<bvh_node>(std::move(world)));


    std::clog << "Rendering scene...\n";
    auto start = std::chrono::high_resolution_clock::now();

    // ✅ 3. 注意：这里传给 render 的是 world_bvh (那棵树)，而不是原来的 world
    cam.render(world_bvh);

    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = stop - start;

    std::clog << "\n--------------------------------------\n";
    std::clog << "Render complete.\n";
    std::clog << "Time taken: " << duration.count() << " seconds.\n";
    std::clog << "--------------------------------------\n";
}