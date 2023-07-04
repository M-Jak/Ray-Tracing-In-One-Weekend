//
// Created by onira on 03/07/2023.
//

#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H

#include "utilities.h"
#include "perlin.h"

class texture {
public:
    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
    solid_color() {}
    solid_color(color c) : color_value(c) {}

    solid_color(double red, double green, double blue)
            : solid_color(color(red,green,blue)) {}

    virtual color value(double u, double v, const vec3& p) const override {
        return color_value;
    }

private:
    color color_value;
};


class noise_texture : public texture {
public:
    noise_texture() {}
    noise_texture(double sc) : scale(sc) {}

    virtual color value(double u, double v, const point3& p) const override {
        return color(1,1,1) * 0.5 * (1 + sin(scale*p.z() + 10*noise.turb(p)));
    }

public:
    perlin noise;
    double scale;
};

#endif //RAYTRACING_TEXTURE_H
