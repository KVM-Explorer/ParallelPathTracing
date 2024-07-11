#pragma once
#include <array>
#include <intrin.h>
#include <tuple>

namespace SIMD {
using Float = float;

#define SHUFFLE4(V, X, Y, Z, W) Float4(_mm_shuffle_ps((V).data, (V).data, _MM_SHUFFLE(W, Z, Y, X)))

struct Float4 {
    inline Float4() {}

    // scalar初始化
    explicit inline Float4(Float f) {
        data = _mm_set1_ps(f);
    }

    explicit inline Float4(const std::array<Float, 4> &f) {
        data = _mm_set_ps(f[0], f[2], f[1], f[0]);
    }

    explicit inline Float4(Float x, Float y, Float z, Float w) {
        data = _mm_set_ps(w, z, y, x);
    }

    explicit inline Float4(__m128 f) {
        data = f;
    }

    // clang-format off
    inline Float x() const { return _mm_cvtss_f32(data);}
    inline Float y() const { return _mm_cvtss_f32(_mm_shuffle_ps(data, data, 0x55));}
    inline Float z() const { return _mm_cvtss_f32(_mm_shuffle_ps(data, data, 0xaa));}
    inline Float w() const { return _mm_cvtss_f32(_mm_shuffle_ps(data, data, 0xff));}
    // clang-format on

    static int Size() { return 4; }

    // operator
    // clang-format off


    // Returns a 4-bit code where bit0..bit3 is X..W
    inline unsigned mask() { return _mm_movemask_ps(data); }
    inline bool any() { return mask() != 0; }  // used for bool4
    inline bool all() { return mask() == 15; } // used for bool4
    // clang-format on

    __m128 data;
};

using bool4 = Float4;

// clang-format off
inline Float4 operator+ (Float4 a, Float4 b) { a.data = _mm_add_ps(a.data, b.data); return a; }
inline Float4 operator- (Float4 a, Float4 b) { a.data = _mm_sub_ps(a.data, b.data); return a; }
inline Float4 operator* (Float4 a, Float4 b) { a.data = _mm_mul_ps(a.data, b.data); return a; }
inline bool4 operator==(Float4 a, Float4 b) { a.data = _mm_cmpeq_ps(a.data, b.data); return a; }
inline bool4 operator!=(Float4 a, Float4 b) { a.data = _mm_cmpneq_ps(a.data, b.data); return a; }
inline bool4 operator< (Float4 a, Float4 b) { a.data = _mm_cmplt_ps(a.data, b.data); return a; } 
inline bool4 operator> (Float4 a, Float4 b) { a.data = _mm_cmpgt_ps(a.data, b.data); return a; }
inline bool4 operator<=(Float4 a, Float4 b) { a.data = _mm_cmple_ps(a.data, b.data); return a; }
inline bool4 operator>=(Float4 a, Float4 b) { a.data = _mm_cmpge_ps(a.data, b.data); return a; }
inline bool4 operator&(bool4 a, bool4 b) { a.data = _mm_and_ps(a.data, b.data); return a; }
inline bool4 operator|(bool4 a, bool4 b) { a.data = _mm_or_ps(a.data, b.data); return a; }
inline Float4 operator- (Float4 a) { a.data = _mm_xor_ps(a.data, _mm_set1_ps(-0.0f)); return a; }
inline Float4 min(Float4 a, Float4 b) { a.data = _mm_min_ps(a.data, b.data); return a; }
inline Float4 max(Float4 a, Float4 b) { a.data = _mm_max_ps(a.data, b.data); return a; }
inline Float Tmin(Float4 v)
{
    v = min(v, SIMD::SHUFFLE4(v, 2, 3, 0, 0));
    v = min(v, SIMD::SHUFFLE4(v, 1, 0, 0, 0));
    return v.x();
}
// clang-format on

struct Float8 {

    static int Size() { return 8; }
};

struct Float16 {

    static int Size() { return 16; }
};

} // namespace SIMD