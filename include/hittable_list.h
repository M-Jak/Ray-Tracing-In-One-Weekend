//
// Created by onira on 26/06/2023.
//

#ifndef RAYTRACING_HITTABLE_LIST_H
#define RAYTRACING_HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
public:
    hittable_list(){}
    hittable_list(shared_ptr<hittable> object) {add(object);}

    void clear() {objects.clear();}
    void add(shared_ptr<hittable> object) { objects.push_back(object);}

    virtual bool hit(const ray& r, double t_min, double t_max, hitRecord& rec) const override;

public:
    std::vector<shared_ptr<hittable>> objects;
};


bool hittable_list::hit(const ray &r, double t_min, double t_max, hitRecord &rec) const {
    hitRecord tempRec;
    bool hitAnything = false;
    auto closestSoFar = t_max;

    for(const auto& object : objects){
        if(object->hit(r, t_min, closestSoFar, tempRec)){
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }

    return hitAnything;
}
#endif //RAYTRACING_HITTABLE_LIST_H
