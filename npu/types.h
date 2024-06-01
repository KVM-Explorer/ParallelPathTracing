#pragma once
#include "kernel_operator.h"

#define FLOAT_TYPE_HALF
#ifdef FLOAT_TYPE_HALF
using FF = float;
#else
using FF = double;
#endif

enum MaterialType { DIFF,
                    SPEC,
                    REFR };

struct Vec {
    FF x, y, z;
    bool operator<(const Vec& other) const {
        // Implement the comparison logic for your Vec class
        // For example, you could compare the individual elements of the vectors
        
        return false; // Vectors are equal
    }
    __ai_host__ __aicore__ Vec(FF x_ = 0, FF y_ = 0, FF z_ = 0)
        : x(x_), y(y_), z(z_) {}
    __ai_host__ __aicore__ Vec operator+(const Vec &v) const {
        return Vec(x + v.x, y + v.y, z + v.z);
    };
    __ai_host__ __aicore__ Vec operator-(const Vec &v) const {
        return Vec(x - v.x, y - v.y, z - v.z);
    };
    __ai_host__ __aicore__ Vec operator*(FF b) const {
        return Vec(x * b, y * b, z * b);
    };
    __ai_host__ __aicore__ Vec mult(const Vec &v) const {
        return Vec(x * v.x, y * v.y, z * v.z);
    }
    __ai_host__ __aicore__ Vec &norm() {
        return *this = *this * (1 / sqrt(x * x + y * y + z * z));
    }
    __ai_host__ __aicore__ FF dot(const Vec &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    __ai_host__ __aicore__ Vec cross(const Vec &v) const {
        return Vec(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

struct Ray {
    Vec o, d;
    __ai_host__ __aicore__ Ray(Vec o_, Vec d_) : o(o_), d(d_) {}
};