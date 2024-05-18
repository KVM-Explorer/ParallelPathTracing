#pragma once
#include "../../../utils/types.h"
#include <cmath>
#include <intrin.h>
#include <tuple>

namespace SIMD {
using Float = float;

struct VecSSE {
    __m128 m_data;

    VecSSE(Float x = 0, Float y = 0, Float z = 0) : m_data(_mm_set_ps(0, z, y, x)) {}
    VecSSE(__m128 data) : m_data(data) {}

    VecSSE operator+(const VecSSE &b) const { return _mm_add_ps(m_data, b.m_data); }
    VecSSE operator-(const VecSSE &b) const { return _mm_sub_ps(m_data, b.m_data); }
    VecSSE operator*(Float b) const { return _mm_mul_ps(m_data, _mm_set1_ps(b)); }
    VecSSE mult(const VecSSE &b) const { return _mm_mul_ps(m_data, b.m_data); }

    // 基于SIMD计算，快但是存在一定精度损失
    VecSSE &normSSE() {
        // TODO: 存在一定的精度损失
        // 水平加法损失精度，导致结果失真
        __m128 squared = _mm_mul_ps(m_data, m_data); // 每个元素平方
        __m128 temp = _mm_hadd_ps(squared, squared); // 水平加法
        temp = _mm_hadd_ps(temp, temp);              // 再次水平加法
        m_data = _mm_mul_ps(m_data, _mm_rsqrt_ps(temp));
        return *this;      
    }

    // Vec &norm() { return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }

    // 直接取出数据计算
    VecSSE &norm() {
        Float values[4];
        _mm_storeu_ps(values, m_data);
        Float length = std::sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
        m_data = _mm_mul_ps(m_data, _mm_set1_ps(1 / length));
        return *this;
    }

    Float dot(const VecSSE &b) const {
        __m128 temp = _mm_mul_ps(m_data, b.m_data);
        temp = _mm_hadd_ps(temp, temp);
        temp = _mm_hadd_ps(temp, temp);
        Float result[4];
        _mm_storeu_ps(result, temp);
        return result[0];
    }

    VecSSE operator%(const VecSSE &b) const {
        __m128 temp1 = _mm_shuffle_ps(m_data, m_data, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 temp2 = _mm_shuffle_ps(b.m_data, b.m_data, _MM_SHUFFLE(3, 1, 0, 2));
        __m128 temp3 = _mm_shuffle_ps(m_data, m_data, _MM_SHUFFLE(3, 1, 0, 2));
        __m128 temp4 = _mm_shuffle_ps(b.m_data, b.m_data, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 result = _mm_sub_ps(_mm_mul_ps(temp1, temp2), _mm_mul_ps(temp3, temp4));
        return result;
    }

    Vec get() const {
        Float values[4];
        _mm_storeu_ps(values, m_data);
        return Vec(values[0], values[1], values[2]);
    }

    Float x() const {
        Float values[4];
        _mm_storeu_ps(values, m_data);
        return values[0];
    }

    Float y() const {
        Float values[4];
        _mm_storeu_ps(values, m_data);
        return values[1];
    }

    Float z() const {
        Float values[4];
        _mm_storeu_ps(values, m_data);
        return values[2];
    }
};

// AVX
struct Vec3X2 {
    __m256 m_data;

    Vec3X2(Float x1 = 0, Float y1 = 0, Float z1 = 0, Float x2 = 0, Float y2 = 0, Float z2 = 0) : m_data(_mm256_set_ps(0, z2, y2, x2, 0, z1, y1, x1)) {}
    Vec3X2(__m256 data) : m_data(data) {}

    Vec3X2 operator+(const Vec3X2 &b) const { return _mm256_add_ps(m_data, b.m_data); }
    Vec3X2 operator-(const Vec3X2 &b) const { return _mm256_sub_ps(m_data, b.m_data); }
    Vec3X2 operator*(Float b) const { return _mm256_mul_ps(m_data, _mm256_set1_ps(b)); }
    Vec3X2 mult(const Vec3X2 &b) const { return _mm256_mul_ps(m_data, b.m_data); }

    Float normSSE() const {
        __m256 squared = _mm256_mul_ps(m_data, m_data); // 每个元素平方
        __m256 temp = _mm256_hadd_ps(squared, squared); // 水平加法
        temp = _mm256_hadd_ps(temp, temp);              // 再次水平加法
        Float result[8];
        _mm256_storeu_ps(result, temp); // 存储结果到数组
        return std::sqrt(result[0]);
    }

    Float norm() const {
        Float values[8];
        _mm256_storeu_ps(values, m_data);
        return std::sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
    }

    Float dot(const Vec3X2 &b) const {
        __m256 temp = _mm256_mul_ps(m_data, b.m_data);
        temp = _mm256_hadd_ps(temp, temp);
        temp = _mm256_hadd_ps(temp, temp);
        Float result[8];
        _mm256_storeu_ps(result, temp);
        return result[0];
    }

    Vec3X2 operator%(const Vec3X2 &b) const {
        __m256 temp1 = _mm256_shuffle_ps(m_data, m_data, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 temp2 = _mm256_shuffle_ps(b.m_data, b.m_data, _MM_SHUFFLE(3, 1, 0, 2));
        __m256 temp3 = _mm256_shuffle_ps(m_data, m_data, _MM_SHUFFLE(3, 1, 0, 2));
        __m256 temp4 = _mm256_shuffle_ps(b.m_data, b.m_data, _MM_SHUFFLE(3, 0, 2, 1));
        __m256 result = _mm256_sub_ps(_mm256_mul_ps(temp1, temp2), _mm256_mul_ps(temp3, temp4));
        return result;
    }

    std::tuple<Vec, Vec> get() const {
        Float values[8];
        _mm256_storeu_ps(values, m_data);
        return {Vec(values[0], values[1], values[2]), Vec(values[4], values[5], values[6])};
    }
};

// AVX512
struct Vec3X4 {
    __m512 m_data;

    Vec3X4(Float x1 = 0, Float y1 = 0, Float z1 = 0, Float x2 = 0, Float y2 = 0, Float z2 = 0, Float x3 = 0, Float y3 = 0, Float z3 = 0, Float x4 = 0, Float y4 = 0, Float z4 = 0) : m_data(_mm512_set_ps(0, z4, y4, x4, 0, z3, y3, x3, 0, z2, y2, x2, 0, z1, y1, x1)) {}

    Vec3X4(__m512 data) : m_data(data) {}

    Vec3X4 operator+(const Vec3X4 &b) const { return _mm512_add_ps(m_data, b.m_data); }
    Vec3X4 operator-(const Vec3X4 &b) const { return _mm512_sub_ps(m_data, b.m_data); }
    Vec3X4 operator*(Float b) const { return _mm512_mul_ps(m_data, _mm512_set1_ps(b)); }
    Vec3X4 mult(const Vec3X4 &b) const { return _mm512_mul_ps(m_data, b.m_data); }
};

} // namespace SIMD