#pragma once
#include "rt_math.h"
#include "utils/rt_helper.h"
#include "utils/types.h"
#include <array>
#include <tuple>

namespace SIMD {

using Scene = std::vector<SIMD::Sphere>;

// 判断是否命中物体
inline bool intersect(const RaySSE &r, Float &t, int &hit_object, Scene &scene) {

    Float min_dis = t = INF;

    for (int i = scene.size(); i--;) {

        min_dis = scene[i].intersect(r);
        if (min_dis && min_dis < t) {
            t = min_dis;
            hit_object = i;
        }
    }
    return t < INF;
}

inline bool intersectX4(const RaySSE &r, Float &t, int &hit_object, Scene &scene) {
    Float min_dis = t = INF;

    for (int i = scene.size(); i -= 4;) {
        // bind
        Vec3X4 v;

        // compute

        // store
    }
    if (scene.size() % 4 != 0) {
        // bind

        // compute

        // store
    }
    return t < INF;
}

} // namespace SIMD