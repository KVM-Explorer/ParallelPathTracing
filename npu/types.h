#pragma once
#include "kernel_operator.h"
#include <random>

#define FLOAT_TYPE_HALF
#ifdef FLOAT_TYPE_HALF
using FF = float;
#else
using FF = double;
#endif

const int32_t WIDTH = 16; // min 16 继续小可能涉及数据对齐问题，无法通过验证
const int32_t HEIGHT = 16; // min 16
const int32_t SAMPLES = 1; // SAMPLES * 4 = total samples

enum MaterialType { DIFF, SPEC, REFR };

struct Vec {
    FF x, y, z, w;
    bool operator<(const Vec &other) const {
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

__ai_host__ __aicore__ static FF randomFF() {
    static std::uniform_real_distribution<FF> dist(0, 1);
    static std::mt19937_64 gen;
    return dist(gen);
}

__ai_host__ __aicore__ static Vec randomVec() {
    return Vec(randomFF(), randomFF(), randomFF());
}

struct Sphere {
    FF rad;
    Vec p, e, c;
    MaterialType m;
    __ai_host__ __aicore__ Sphere(FF rad_, Vec p_, Vec e_, Vec c_,
                                   MaterialType m_)
        : rad(rad_), p(p_), e(e_), c(c_), m(m_) {}
    __ai_host__ __aicore__ FF intersect(const Ray &r) const {
        Vec op = p - r.o;
        FF t, epsilon = 1e-4, b = op.dot(r.d),
              det = b * b - op.dot(op) + rad * rad;
        if (det < FF(0))
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > epsilon ? t
                                       : ((t = b + det) > epsilon ? t : FF(0));
    }
};