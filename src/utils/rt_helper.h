
#pragma once
#include "types.h"
#include <cmath>

struct Ray {
    Vec origin;
    Vec dir;

    Ray(Vec origin, Vec direction) : origin(origin), dir(direction) {}
    Ray() {}
    Vec at(Float t) const { return origin + dir * t; }
};

struct Sphere {
    Float radius;
    Vec position;
    Vec emission;
    Vec color;
    MaterialType material;

    Sphere(Float radius, Vec position, Vec emission, Vec color, MaterialType material)
        : radius(radius), position(position), emission(emission), color(color), material(material) {}

    Float intersect(const Ray &ray) const {
        Vec op = position - ray.origin;
        Float t, eps = 1e-4, b = op.dot(ray.dir), det = b * b - op.dot(op) + radius * radius;
        if (det < 0)
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }
};

