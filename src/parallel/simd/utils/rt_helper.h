#pragma once
#include "types.h"

namespace SIMD {

struct Ray {
    using Vec = SIMD::VecSSE;
    Vec origin;
    Vec dir;

    Ray(Vec origin, Vec direction) : origin(origin), dir(direction) {}
};

struct Sphere {
    using Vec = SIMD::VecSSE;
    Float radius;
    Vec position;
    Vec emission;
    Vec color;
    MaterialType material;

    Sphere(Float radius, Vec position, Vec emission, Vec color, MaterialType material)
        : radius(radius), position(position), emission(emission), color(color), material(material) {}

    Float intersect(const Ray &ray) const {
        Vec op = position - ray.origin;
        Float t, eps = 1e-4;
        Float b = op.dot(ray.dir);
        Float det = b * b - op.dot(op) + radius * radius;
        if (det < 0)
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }
};

struct Camera {
    using Vec = SIMD::VecSSE;
    Vec position;
    Vec direction;
    Vec cx, cy;

    Camera(Vec position, Vec direction, int w, int h) : position(position), direction(direction) {
        cx = Vec(w * .5135 / h);
        cy = (cx % direction).norm() * .5135;
    }
};

} // namespace SIMD
