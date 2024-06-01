#pragma once
#include "types.h"
#include <random>
#include <vector>

FF randomFF() {
    std::uniform_real_distribution<FF> dist(0, 1);
    std::mt19937 gen;
    return dist(gen);
}

Vec randomVec() {
    return Vec(randomFF(), randomFF(), randomFF());
}

auto genRays(int numRays) -> std::vector<Ray> {
    std::vector<Ray> rays;
    for (int i = 0; i < numRays; i++) {
        Vec o = randomVec();
        Vec d = randomVec();
        rays.push_back(Ray(o, d));
    }
    return rays;
}

auto stdColor(const std::vector<Ray> &rays) -> std::vector<Vec> {
    std::vector<Vec> colors;
    for (const auto &ray : rays) {
        Vec color = ray.o;
        colors.push_back(color);
    }
    return colors;
}