#include "catch2/catch_amalgamated.hpp"

#include "../src/parallel/simd/utils/rt_helper.h"
#include "../src/parallel/simd/utils/types.h"
#include "../src/utils/rt_helper.h"

// TEST_CASE("sse::type", "[SSE]") {
//     using namespace SIMD;
//     VecSSE a = VecSSE(1, 2, 3);
//     VecSSE b = VecSSE(4, 5, 6);
//     VecSSE c = a + b;

//     REQUIRE(c.x() == 5);
//     REQUIRE(c.y() == 7);
//     REQUIRE(c.z() == 9);
// }

// TEST_CASE("sse::norm", "[SSE]") {
//     using namespace SIMD;

//     VecSSE c = VecSSE(1, 2, 3);
//     auto d = c.norm().get();
//     REQUIRE(d.x == Catch::Approx(0.26726124).margin(0.0001));
//     REQUIRE(d.y == Catch::Approx(0.53452248).margin(0.0001));
//     REQUIRE(d.z == Catch::Approx(0.80178373).margin(0.0001));

//     VecSSE a = VecSSE(1, 2, 3);
//     auto b = a.normSSE().get();
//     REQUIRE(b.x == Catch::Approx(0.26726124).margin(0.0001));
//     REQUIRE(b.y == Catch::Approx(0.53452248).margin(0.0001));
//     REQUIRE(b.z == Catch::Approx(0.80178373).margin(0.001)); // SIMD 计算存在精度损失
// }

// TEST_CASE("sse::mult", "[SSE]") {
//     using namespace SIMD;
//     VecSSE a = VecSSE(1, 2, 3);
//     VecSSE b = VecSSE(4, 5, 6);
//     VecSSE c = a.mult(b);

//     REQUIRE(c.x() == 4);
//     REQUIRE(c.y() == 10);
//     REQUIRE(c.z() == 18);
// }

// TEST_CASE("sse::operator*", "[SSE]") {
//     using namespace SIMD;
//     VecSSE a = VecSSE(1, 2, 3);
//     VecSSE b = a * 2;

//     REQUIRE(b.x() == 2);
//     REQUIRE(b.y() == 4);
//     REQUIRE(b.z() == 6);
// }

// TEST_CASE("sse::operator-", "[SSE]") {
//     using namespace SIMD;
//     VecSSE a = VecSSE(1, 2, 3);
//     VecSSE b = VecSSE(4, 5, 6);
//     VecSSE c = a - b;

//     REQUIRE(c.x() == -3);
//     REQUIRE(c.y() == -3);
//     REQUIRE(c.z() == -3);
// }

// TEST_CASE("sse::operator+", "[SSE]") {
//     using namespace SIMD;
//     VecSSE a = VecSSE(1, 2, 3);
//     VecSSE b = VecSSE(4, 5, 6);
//     VecSSE c = a + b;

//     REQUIRE(c.x() == 5);
//     REQUIRE(c.y() == 7);
//     REQUIRE(c.z() == 9);
// }

// TEST_CASE("sse::dot", "[SSE]") {
//     using namespace SIMD;
//     VecSSE a = VecSSE(1, 2, 3);
//     VecSSE b = VecSSE(4, 5, 6);
//     REQUIRE(a.dot(b) == 32);
// }

// TEST_CASE("sse::cross", "[SSE]") {
//     using namespace SIMD;
//     VecSSE a = VecSSE(1, 2, 3);
//     VecSSE b = VecSSE(4, 5, 6);
//     VecSSE c = a % b;

//     REQUIRE(c.x() == -3);
//     REQUIRE(c.y() == 6);
//     REQUIRE(c.z() == -3);
// }
// // ==================== Performance Test ====================

// TEST_CASE("sse::basic operation", "[performance]") {
//     using namespace SIMD;
//     VecSSE a = VecSSE(1, 2, 3);
//     VecSSE b = VecSSE(4, 5, 6);
//     BENCHMARK("add") { return a + b; };
//     BENCHMARK("mult") { return a.mult(b); };
//     BENCHMARK("operator*") { return a * 2; };
//     BENCHMARK("norm") { return a.norm(); };
//     BENCHMARK("normSSE") { return a.normSSE(); };
//     BENCHMARK("dot") { return a.dot(b); };
//     BENCHMARK("cross") { return a % b; };
// }

TEST_CASE("sse::spere intersect", "[performance]") {
    SIMD::Sphere sphere(12, {2, 3, 4}, {}, {}, MaterialType::DIFFUSE);
    SIMD::RaySSE r({1, 2, 3}, {4, 5, 6});

    Sphere cpu_sphere(12, {2, 3, 4}, {}, {}, MaterialType::DIFFUSE);
    RaySSE cpu_r({1, 2, 3}, {4, 5, 6});

    BENCHMARK("cpu sphere inersect") {
        return cpu_sphere.intersect(cpu_r);
    };
    BENCHMARK("simd sphere intersect | vec only") {
        return sphere.intersect(r);
    };
    BENCHMARK("simd sphere intersect | simd algriothmn") {
        return sphere.intersectSIMD(r);
    };
}
