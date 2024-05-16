#pragma once
#include <cmath>

struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
    double x, y, z; // position, also color (r,g,b)
    Vec(double x_ = 0, double y_ = 0, double z_ = 0) {
        x = x_;
        y = y_;
        z = z_;
    }
    Vec operator+(const Vec &b) const { return Vec(x + b.x, y + b.y, z + b.z); }
    Vec operator-(const Vec &b) const { return Vec(x - b.x, y - b.y, z - b.z); }
    Vec operator*(double b) const { return Vec(x * b, y * b, z * b); }
    Vec mult(const Vec &b) const { return Vec(x * b.x, y * b.y, z * b.z); }
    Vec &norm() { return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }
    double dot(const Vec &b) const { return x * b.x + y * b.y + z * b.z; } // cross:
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
    double radius;
    Vec position;
    Vec emission;
    Vec color;
    MaterialType material;

    Sphere(double radius, Vec position, Vec emission, Vec color, MaterialType material)
        : radius(radius), position(position), emission(emission), color(color), material(material) {}

    double intersect(const Ray &ray) const {
        Vec op = position - ray.origin;
        double t, eps = 1e-4, b = op.dot(ray.dir), det = b * b - op.dot(op) + radius * radius;
        if (det < 0)
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }
};