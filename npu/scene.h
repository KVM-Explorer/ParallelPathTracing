#pragma once
#include "types.h"
#include <vector>

static auto inline cornelbox() -> std::vector<Sphere> {
    return {Sphere(1e5, Vec(1e5 + 1, 40.8, 81.6), Vec(), Vec(.75, .25, .25),
                   DIFF), // Left
            Sphere(1e5, Vec(-1e5 + 99, 40.8, 81.6), Vec(), Vec(.25, .25, .75),
                   DIFF), // Right
            Sphere(1e5, Vec(50, 40.8, 1e5), Vec(), Vec(.75, .75, .75), DIFF),
            Sphere(1e5, Vec(50, 40.8, -1e5 + 170), Vec(), Vec(), DIFF),
            Sphere(1e5, Vec(50, 1e5, 81.6), Vec(), Vec(.75, .75, .75), DIFF),
            Sphere(1e5, Vec(50, -1e5 + 81.6, 81.6), Vec(), Vec(.75, .75, .75),
                   DIFF),
            Sphere(16.5, Vec(27, 16.5, 47), Vec(), Vec(1, 1, 1) * .999,
                   SPEC), // Mirror
            Sphere(16.5, Vec(73, 16.5, 78), Vec(), Vec(1, 1, 1) * .999,
                   REFR), // Glass
            Sphere(600, Vec(50, 681.6 - .27, 81.6), Vec(12, 12, 12), Vec(),
                   DIFF)}; // Light
};
