#pragma once
#include "string"
#include "types.h"
#include <format>
#include <iostream>
#include <random>


using Scene = std::vector<Sphere>;

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.14159265358979323846;

inline double clamp(double x) { return x < 0 ? 0 : x > 1 ? 1
                                                         : x; }

inline void OutputStatus(const std::string name, int y, int h, int sapmles, int w) {
    std::cout << std::format("\rRendering ({0} spp) {1:5.2f}%", sapmles * 4, 100. * y / (h - 1));
}

inline double random() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937_64 generator;
    return distribution(generator);
}

// 判断是否命中物体
inline bool intersect(const Ray &r, double &t, int &hit_object, Scene &scene) {

    double min_dis = t = INF;

    for (int i = scene.size(); i--;) {

        min_dis = scene[i].intersect(r);
        if (min_dis && min_dis < t) {
            t = min_dis;
            hit_object = i;
        }
    }
    return t < INF;
}
