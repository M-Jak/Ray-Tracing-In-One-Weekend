#include "../include/rtweekend.h"

#include "../include/color.h"
#include "../include/hittable_list.h"
#include "../include/sphere.h"

#include <iostream>

color rayColor(const ray& r, const hittable& world) {
    hitRecord rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1,1,1));
    }

    vec3 unit_direction = unitVector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    //image
    const auto aspectRatio = 16.0/9.0;
    const int imageWidth = 400;
    const int imageHeight = static_cast<int>(imageWidth/aspectRatio);

    //world
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    //camera
    auto viewportHeight = 2.0;
    auto viewportWidth = aspectRatio * viewportHeight;
    auto focalLength = 1.0;

    auto origin = point3(0,0,0);
    auto horizontal = vec3(viewportWidth,0,0);
    auto vertical = vec3(0,viewportHeight,0);
    auto lowerLeftCorner = origin - horizontal/2 - vertical/2 - vec3(0,0,focalLength);


    //render

    std::cout<<"P3\n" << imageWidth << ' ' << imageHeight<<"\n255\n";

    for(int j = imageHeight-1; j>=0; j--){
        std::cerr<<"\rScanlines remaining: " << j << ' '<<std::flush;
        for (int i = 0; i < imageWidth; ++i){
            auto u = double(i)/(imageWidth-1);
            auto v = double(j)/(imageHeight-1);
            ray r(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
            color pixelColor = rayColor(r, world);

            writeColor(std::cout, pixelColor);
        }
    }
    std::cerr<<"\nDone.\n";
}
