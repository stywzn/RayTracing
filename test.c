void render(const hittable &world)
    {
        initialize();

        std::cout << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\rScanlines remaining: " << (j*100)/image_height << '%' << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << "\rDone.                       \n";
    }
