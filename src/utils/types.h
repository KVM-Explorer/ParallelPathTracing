#pragma once
#include <cmath>

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
