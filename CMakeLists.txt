cmake_minimum_required(VERSION 3.25)
project(RayTracing)

set(CMAKE_CXX_STANDARD 17)

add_executable(RayTracing src/main.cpp include/vec3.h include/color.h include/ray.h include/hittable.h include/sphere.h include/hittable_list.h include/utilities.h include/camera.h include/material.h)
