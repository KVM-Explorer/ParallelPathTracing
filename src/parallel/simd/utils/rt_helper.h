#pragma once
#include "types.h"

namespace SIMD {

struct Ray {
    using Vec = SIMD::VecSSE;
    Vec origin;
    Vec dir;

    Ray(Vec origin, Vec direction) : origin(origin), dir(direction) {}
};

struct RayX2 {
    using Vec = SIMD::VecSSE;
    Vec3X2 origin;
    Vec3X2 dir;

    RayX2(Ray ray1, Ray ray2)
        : origin(ray1.origin, ray2.origin), dir(ray1.dir, ray2.dir) {}
    RayX2(Vec origin1, Vec direction1, Vec origin2, Vec direction2)
        : origin(origin1, origin2), dir(direction1, direction2) {}
};

struct RayX4 {
    using Vec = SIMD::VecSSE;
    Vec3X4 origin;
    Vec3X4 dir;

    // RayX4(Vec origin1, Vec direction1, Vec origin2, Vec direction2, Vec origin3, Vec direction3, Vec origin4, Vec direction4)
    //     : origin(origin1, origin2, origin3, origin4),
    //       dir(direction1, direction2, direction3, direction4) {}
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
