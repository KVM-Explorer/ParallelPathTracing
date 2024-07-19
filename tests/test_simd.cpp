#include "catch2/catch_amalgamated.hpp"

#include "parallel/simd/math_simd.h"

// ==================== Unit Tests ====================

TEST_CASE("SIMD: Add", "[simd-float4]") {
    SIMD::Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
    SIMD::Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
    SIMD::Float4 c = a + b;

    REQUIRE(c.x() == 6.0f);
    REQUIRE(c.y() == 8.0f);
    REQUIRE(c.z() == 10.0f);
    REQUIRE(c.w() == 12.0f);
}

TEST_CASE("SIMD: Subtract", "[simd-float4]") {
    SIMD::Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
    SIMD::Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
    SIMD::Float4 c = a - b;

    REQUIRE(c.x() == -4.0f);
    REQUIRE(c.y() == -4.0f);
    REQUIRE(c.z() == -4.0f);
    REQUIRE(c.w() == -4.0f);
}

TEST_CASE("SIMD: Multiply", "[simd-float4]") {
    SIMD::Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
    SIMD::Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
    SIMD::Float4 c = a * b;

    REQUIRE(c.x() == 5.0f);
    REQUIRE(c.y() == 12.0f);
    REQUIRE(c.z() == 21.0f);
    REQUIRE(c.w() == 32.0f);
}

TEST_CASE("SIMD::min", "[simd-float4]") {
    SIMD::Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
    SIMD::Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
    SIMD::Float4 c = SIMD::min(a, b);

    REQUIRE(c.x() == 1.0f);
    REQUIRE(c.y() == 2.0f);
    REQUIRE(c.z() == 3.0f);
    REQUIRE(c.w() == 4.0f);
}

TEST_CASE("SIMD::max", "[simd-float4]") {
    SIMD::Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
    SIMD::Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
    SIMD::Float4 c = SIMD::max(a, b);

    REQUIRE(c.x() == 5.0f);
    REQUIRE(c.y() == 6.0f);
    REQUIRE(c.z() == 7.0f);
    REQUIRE(c.w() == 8.0f);
}

TEST_CASE("SIMD::Tmin", "[simd-float4]") {
    SIMD::Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
    REQUIRE(SIMD::Tmin(a) == 1.0f);
}

TEST_CASE("SIMD::Float4::mask", "[simd-float4]") {

    SIMD::Float4 b(0.0f, 0.0f, 0.0f, 0.0f);
    REQUIRE(b.mask() == 0);

    SIMD::Float4 c(1.0f, 0.0f, 0.0f, 0.0f);
    REQUIRE(c.mask() == 1);

    SIMD::Float4 d(0.0f, 1.0f, 0.0f, 0.0f);
    REQUIRE(d.mask() == 2);

    SIMD::Float4 e(0.0f, 0.0f, 1.0f, 0.0f);
    REQUIRE(e.mask() == 4);

    SIMD::Float4 f(0.0f, 0.0f, 0.0f, 1.0f);
    REQUIRE(f.mask() == 8);

    SIMD::Float4 g(1.0f, 1.0f, 1.0f, 1.0f);
    REQUIRE(g.mask() == 15);
}

TEST_CASE("SIMD::Float4::any", "[simd-float4]") {
    SIMD::Float4 a(1, 0, 0, 0);
    REQUIRE(a.any() == true);

    SIMD::Float4(0, 1, 0, 0);
    REQUIRE(a.any() == true);

    SIMD::Float4 b(0, 0, 0, 0);
    REQUIRE(b.any() == false);
}

TEST_CASE("SIMD::Float4::all", "[simd-float4]") {
    SIMD::Float4 a(1, 1, 1, 1);
    REQUIRE(a.all() == true);
}

TEST_CASE("SIMD::Float4::Size", "[simd-float4]") {
    REQUIRE(SIMD::Float4::Size() == 4);
}

// ==================== Performance Test ====================
