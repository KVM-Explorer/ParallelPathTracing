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

inline bool intersetX2(const RaySSE &r, Float &t, int &hit_object, Scene &scene) {
    Float min_dis = t = INF;

    for (int i = scene.size(); i -= 2;) {
        // bind
        RayX2 ray(r, r);
        // Vec2 ret = intersetGroup({&scene[i], &scene[i-1]}, ray);

        // compute

        // store
    }
    if (scene.size() % 2 == 1) {
    }

    return t < INF;
}

inline Vec2 intersetGroup(std::array<SIMD::Sphere *, 2> objects, const RayX2 &r) {
    // Vec3X2 position(objects[0]->position, objects[1]->position);
    // Vec2 radius(objects[0]->radius, objects[1]->radius);
    // Vec3X2 op = position - r.origin;
    // Vec2 b = op.dot(r.dir);
    // Vec2 det = b.mult(b) - op.dot(op) + radius.mult(radius);

    // Vec2 ret;
    // for(int i = 0; i < 2; i++) {
    //     if (det < 0) {
    //         det[i] = 0;
    //     } else {
    //         det[i] = sqrt(det[i]);
    //     }
    // }

    // // Vec3X2 mask = det < 0;
    // // det = sqrt(det);
    // // Vec3X2 t = b - det;
    // // t = t > 1e-4 ? t : (b + det > 1e-4 ? b + det : 0);
    // return
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