cmake_minimum_required(VERSION 3.25)
project(RayTracing)

set(CMAKE_CXX_STANDARD 17)

add_executable(RayTracing src/main.cpp src/vec3.cpp include/vec3.h include/color.h include/ray.h)
