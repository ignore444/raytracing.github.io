//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include <iostream>


color ray_color(const ray& r, const hittable& world, int depth) 
{
    // 배경색이 빛의 역활
    // 배경에 도달하지 못하면, 에너지가 없어서 color가 0이된다.
    // a. depth <= 0 인 경우
    // b. scatter가 없는 경우

    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
    {
        return color(0,0,0);
    }

    if (world.hit(r, 0.001, infinity, rec)) 
    {
        ray     scattered;      // 부딪혀서 나온 ray
        color   attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {   // 부딪혀 나온 ray가 있다
            return attenuation * ray_color(scattered, world, depth-1);
        }
        else
        {   // 부딪혀 나온 ray가 없다
            return color(0, 0, 0);
        }
    }

    // 배경색
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}


hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
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

    return world;
}

namespace _1_4_Rendering_BlueToWhiteGradient
{
    color ray_color(const ray& r) {
        vec3 unit_direction = unit_vector(r.direction());
        auto t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
    }

    void main()
    {
        // Image
        const auto aspect_ratio = 16.0 / 8.0;
        const int image_width = 10;
        const int image_height = static_cast<int>(image_width / aspect_ratio);

        // Camera

        auto viewport_height = 2.0;
        auto viewport_width = aspect_ratio * viewport_height;
        auto focal_length = 1.0;

        auto origin = point3(0, 0, 0);
        auto horizontal = vec3(viewport_width, 0, 0);
        auto vertical = vec3(0, viewport_height, 0);
        auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

        // Render

        std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

        // #_1_04 : Listing 9: [main.cc] Rendering a blue-to-white gradient
        // image 픽셀 수 만큼 ray를 만든다.
        // U/V : [0,1]
        // lower_left_corner : (-2,-1,-1)
        // viewport_width , viewport_height : 4 , 2
        // horizontal   : (4,0,0)
        // vertical     : (0,2,0)
        // r : o (0,0,0)
        //     d (-2,+1,-1) , uv (0,1) 
        //     d (+2,-1,-1) , uv (1,0)
        // pixel_color( color ) : [0,1]
        for (int j = image_height - 1; j >= 0; --j) 
        {
            std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) 
            {
                auto u = double(i) / (image_width - 1);     // 0 -> 1
                auto v = double(j) / (image_height - 1);    // 1 -> 0
                ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
                color pixel_color = ray_color(r);
                write_color(std::cout, pixel_color, 1);
            }
        }

        std::cerr << "\nDone.\n";
    }
};


namespace OneWeekend
{
#define AA 0    // #_1_07 : 하나의 pixel안에 여러 ray => AA

void main() {

    // Image

    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 600; //1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
#if AA
    const int samples_per_pixel = 10;
#else
    const int samples_per_pixel = 1;
#endif
    const int max_depth = 50;

    // World

    auto world = random_scene();

    // Camera

    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    double vertical_fov = 20.0; // degree
    camera cam(lookfrom, lookat, vup, vertical_fov, aspect_ratio, aperture, dist_to_focus);

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) 
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) 
        {
            color pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; ++s) 
            {
            #if AA
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
            #else
				auto u = (double)i / (image_width - 1);
				auto v = (double)j / (image_height - 1);
            #endif

                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
};
void main()
{
	return OneWeekend::main();
}