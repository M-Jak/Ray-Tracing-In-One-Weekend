#include "../include/utilities.h"
#include "../include/camera.h"

#include "../include/color.h"
#include "../include/hittable_list.h"
#include "../include/sphere.h"

#include "../include/material.h"
#include <iostream>
#include <sstream>
#include <thread>

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

hittable_list random_scene(){
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5,0.5,0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));


    for (int a = -11; a < 11; a++){
        for (int b = -11; b < 11; b++){
            auto choose_mat = random_double();
            point3 center(a+0.9*random_double(), 0.2, b+0.9*random_double());

            if((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphereMaterial;

                if(choose_mat < 0.8){
                    //difuse
                    auto albedo = color::random() * color::random();
                    sphereMaterial = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center,0.2,sphereMaterial));
                } else if (choose_mat < 0.95){
                    //metal
                    auto albedo = color::random(0.5,1);
                    auto fuzz = random_double(0, 0.5);
                    sphereMaterial = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
                } else {
                    //glass
                    sphereMaterial = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center,0.2,sphereMaterial));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0,1,0), 1.0, material1));
    auto material2 = make_shared<lambertian>(color(0.4,0.2,0.1));
    world.add(make_shared<sphere>(point3(-4,1,0), 1.0, material2));
    auto material3 = make_shared<metal>(color(0.7,0.6,0.5), 0.0);
    world.add(make_shared<sphere>(point3(4,1,0), 1.0, material3));
    return world;
}


hittable_list spiral_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    int numSpirals = 10;  // Number of spiral turns
    int numSpheresPerSpiral = 50;  // Number of spheres per spiral
    double spiralRadius = 4.0;  // Radius of the spiral

    for (int i = 0; i < numSpheresPerSpiral * numSpirals; i++) {
        double t = double(i) / numSpheresPerSpiral;
        double theta = 2.0 * M_PI * t * numSpirals;
        double x = spiralRadius * t * cos(theta);
        double z = spiralRadius * t * sin(theta);
        point3 center(x, 0.2, z);

        // Material selection and sphere creation
        auto choose_mat = random_double();
        shared_ptr<material> sphereMaterial;

        if (choose_mat < 0.8) {
            // Diffuse
            auto albedo = color::random() * color::random();
            sphereMaterial = make_shared<lambertian>(albedo);
        } else if (choose_mat < 0.95) {
            // Metal
            auto albedo = color::random(0.5, 1);
            auto fuzz = random_double(0, 0.5);
            sphereMaterial = make_shared<metal>(albedo, fuzz);
        } else {
            // Glass
            sphereMaterial = make_shared<dielectric>(1.5);
        }

        world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
    }

    //middle sphere
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 0.9, material1));


    return world;
}

std::ostringstream streams[] {std::ostringstream(), std::ostringstream(), std::ostringstream(), std::ostringstream(), std::ostringstream(), std::ostringstream(), std::ostringstream(), std::ostringstream()};

void renderScanlines(int start, int end, int image_width, int image_height, int samples_per_pixel, camera cam, hittable_list world, int max_depth, int t_id) {
    for (int j = start; j >= end; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(streams[t_id], pixel_color, samples_per_pixel);
        }
    }
}

int main() {

    // Image

    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int num_threads = 8;
    const int max_depth = 100;

    std::vector<std::thread> threads;
    int lines_per_thread = image_height/num_threads;
    int start = image_height-1;

    // World
    auto world = spiral_scene();

    // Camera
    point3 lookfrom(0.1,20,0.1);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 23;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 10, aspect_ratio, aperture, dist_to_focus);

    //Threads
    for (int t=0; t<num_threads; ++t){
        int end = start-lines_per_thread+1;
        if (t==num_threads - 1){
            end = 0;
        }

        threads.emplace_back(renderScanlines, start, end, image_width, image_height, samples_per_pixel, cam, world, max_depth, t);

        start = end - 1;
    }

    // Render

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (auto& t : threads) {
        t.join();
    }

    for (const auto & stream : streams){
        std::cout << stream.str();
    }

    std::cerr << "\nDone.\n";

    return 0;
}