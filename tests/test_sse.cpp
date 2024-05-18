#include "catch2/catch_amalgamated.hpp"

#include "../src/parallel/simd/utils/types.h"

TEST_CASE("sse::type", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    VecSSE b = VecSSE(4, 5, 6);
    VecSSE c = a + b;

    REQUIRE(c.x() == 5);
    REQUIRE(c.y() == 7);
    REQUIRE(c.z() == 9);
}

TEST_CASE("sse::norm", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    REQUIRE(a.norm() == Catch::Approx(3.74165738677));
}

TEST_CASE("sse::normSSE", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    REQUIRE(a.normSSE() == Catch::Approx(3.74165738677));
}

TEST_CASE("sse::mult", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    VecSSE b = VecSSE(4, 5, 6);
    VecSSE c = a.mult(b);

    REQUIRE(c.x() == 4);
    REQUIRE(c.y() == 10);
    REQUIRE(c.z() == 18);
}

TEST_CASE("sse::operator*", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    VecSSE b = a * 2;

    REQUIRE(b.x() == 2);
    REQUIRE(b.y() == 4);
    REQUIRE(b.z() == 6);
}

TEST_CASE("sse::operator-", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    VecSSE b = VecSSE(4, 5, 6);
    VecSSE c = a - b;

    REQUIRE(c.x() == -3);
    REQUIRE(c.y() == -3);
    REQUIRE(c.z() == -3);
}

TEST_CASE("sse::operator+", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    VecSSE b = VecSSE(4, 5, 6);
    VecSSE c = a + b;

    REQUIRE(c.x() == 5);
    REQUIRE(c.y() == 7);
    REQUIRE(c.z() == 9);
}

TEST_CASE("sse::dot", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    VecSSE b = VecSSE(4, 5, 6);
    REQUIRE(a.dot(b) == 32);
}

TEST_CASE("sse::cross", "[SSE]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    VecSSE b = VecSSE(4, 5, 6);
    VecSSE c = a % b;

    REQUIRE(c.x() == -3);
    REQUIRE(c.y() == 6);
    REQUIRE(c.z() == -3);
}
// ==================== Performance Test ====================

TEST_CASE("sse::basic operation", "[performance]") {
    using namespace SIMD;
    VecSSE a = VecSSE(1, 2, 3);
    VecSSE b = VecSSE(4, 5, 6);
    BENCHMARK("add") { return a + b; };
    BENCHMARK("mult") { return a.mult(b); };
    BENCHMARK("operator*") { return a * 2; };
    BENCHMARK("norm") { return a.norm(); };
    BENCHMARK("normSSE") { return a.normSSE(); };
    BENCHMARK("dot") { return a.dot(b); };
    BENCHMARK("cross") { return a % b; };
}
