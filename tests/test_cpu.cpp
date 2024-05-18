#include "catch2/catch_amalgamated.hpp"

#include "../src/utils/types.h"

// TEST_CASE("Pixel color test","[pixel_color_test]") {
//     int x = 5;
//     int y = 0;
//     int subx = 0;
//     int suby = 1;
//     int s = 0;
//     Vec d = Vec(-0.311788, -0.274857, -0.909529);
//     Vec tmp = tracing(Ray(Vec(50, 52, 295.6) + d * 140, d.norm()), 0, Scene);
//     REQUIRE( tmp.x == 0.0 );
//     REQUIRE( tmp.y == 0.0 );
//     REQUIRE( tmp.z == 0.0 );
// }

TEST_CASE("cpu::basic operation", "[function]") {
    Vec a = Vec(1, 2, 3);
    Vec b = Vec(4, 5, 6);

    REQUIRE((a + b).x == 5);
    REQUIRE((a + b).y == 7);
    REQUIRE((a + b).z == 9);

    REQUIRE((a - b).x == -3);
    REQUIRE((a - b).y == -3);
    REQUIRE((a - b).z == -3);

    REQUIRE((a * 2).x == 2);
    REQUIRE((a * 2).y == 4);
    REQUIRE((a * 2).z == 6);

    REQUIRE(a.mult(b).x == 4);
    REQUIRE(a.mult(b).y == 10);
    REQUIRE(a.mult(b).z == 18);

    // TODO: Fix the test case
    // REQUIRE(a.norm().x == Catch::Approx(0.26726124));
    // REQUIRE(a.norm().y == Catch::Approx(0.53452248));
    // REQUIRE(a.norm().z == Catch::Approx(0.80178373));

    REQUIRE(a.dot(b) == 32);

    REQUIRE((a % b).x == -3);
    REQUIRE((a % b).y == 6);
    REQUIRE((a % b).z == -3);
}

// ============== Performance Test ==============

TEST_CASE("cpu::basic operation", "[performance]") {
    Vec a = Vec(1, 2, 3);
    Vec b = Vec(4, 5, 6);

    BENCHMARK("add") { return a + b; };
    BENCHMARK("mult") { return a.mult(b); };
    BENCHMARK("operator*") { return a * 2; };
    BENCHMARK("norm") { return a.norm(); };
    BENCHMARK("dot") { return a.dot(b); };
    BENCHMARK("cross") { return a % b; };
}