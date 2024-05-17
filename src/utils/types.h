#pragma once
#include <cmath>
#include <intrin.h>
#include <array>

using Float = float;

struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
    Float x, y, z; // position, also color (r,g,b)
    Vec(Float x_ = 0, Float y_ = 0, Float z_ = 0) {
        x = x_;
        y = y_;
        z = z_;
    }
    Vec operator+(const Vec &b) const { return Vec(x + b.x, y + b.y, z + b.z); }
    Vec operator-(const Vec &b) const { return Vec(x - b.x, y - b.y, z - b.z); }
    Vec operator*(Float b) const { return Vec(x * b, y * b, z * b); }
    Vec mult(const Vec &b) const { return Vec(x * b.x, y * b.y, z * b.z); }
    Vec &norm() { return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }
    Float dot(const Vec &b) const { return x * b.x + y * b.y + z * b.z; } // cross:
    Vec operator%(Vec &b) { return Vec(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
};

enum class MaterialType {
    DIFFUSE,
    SPECULAR,
    REFRACTIVE
};

struct Ray {
    Vec origin;
    Vec dir;

    Ray(Vec origin, Vec direction) : origin(origin), dir(direction) {}
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