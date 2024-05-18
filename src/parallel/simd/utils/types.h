#pragma once
#include "../../../utils/types.h"
#include <cmath>
#include <intrin.h>

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

    Float normSSE() const {
        __m128 squared = _mm_mul_ps(m_data, m_data); // 每个元素平方
        __m128 temp = _mm_hadd_ps(squared, squared); // 水平加法
        temp = _mm_hadd_ps(temp, temp);              // 再次水平加法
        Float result[4];
        _mm_storeu_ps(result, temp); // 存储结果到数组
        return std::sqrt(result[0]);
    }

    Float norm() const {
        Float values[4];
        _mm_storeu_ps(values, m_data);
        return std::sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
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

} // namespace SIMD