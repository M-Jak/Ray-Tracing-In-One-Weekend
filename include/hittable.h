//
// Created by onira on 26/06/2023.
//

#ifndef RAYTRACING_HITTABLE_H
#define RAYTRACING_HITTABLE_H

#include "ray.h"

class material;

struct hit_record{
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outwardNormal){
        front_face = dot(r.direction(), outwardNormal) < 0;
        normal = front_face ? outwardNormal : -outwardNormal;
    }
};


class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif //RAYTRACING_HITTABLE_H
