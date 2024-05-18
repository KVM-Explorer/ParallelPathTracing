#include "catch2/catch_amalgamated.hpp"

#include "../src/parallel/simd/utils/types.h"

TEST_CASE("avx::type", "[AVX]") {
    using namespace SIMD;

    Vec3X2 a = Vec3X2(1, 2, 3, 4, 5, 6);

    Vec3X2 b = Vec3X2(4, 5, 6, 7, 8, 9);

    auto [c0, c1] = (a + b).get();

    REQUIRE(c0.x == 5);
    REQUIRE(c0.y == 7);
    REQUIRE(c0.z == 9);

    REQUIRE(c1.x == 11);
    REQUIRE(c1.y == 13);
    REQUIRE(c1.z == 15);

    auto [d0, d1] = (a - b).get();

    REQUIRE(d0.x == -3);
    REQUIRE(d0.y == -3);
    REQUIRE(d0.z == -3);

    REQUIRE(d1.x == -3);
    REQUIRE(d1.y == -3);
    REQUIRE(d1.z == -3);

    auto [e0, e1] = (a * 2).get();

    REQUIRE(e0.x == 2);
    REQUIRE(e0.y == 4);
    REQUIRE(e0.z == 6);

    REQUIRE(e1.x == 8);
    REQUIRE(e1.y == 10);
    REQUIRE(e1.z == 12);

    auto [f0, f1] = a.mult(b).get();

    REQUIRE(f0.x == 4);
    REQUIRE(f0.y == 10);
    REQUIRE(f0.z == 18);

    REQUIRE(f1.x == 28);
    REQUIRE(f1.y == 40);
    REQUIRE(f1.z == 54);

    Vec2 u = a.dot(b);

    REQUIRE(u[0] == 32);
    REQUIRE(u[1] == 122);
}