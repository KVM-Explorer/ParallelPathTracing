#pragma once
#include "parallel/simd/math_simd.h"
#include "rt_math.h"
#include "scene.h"
#include "utils/rt_helper.h"
#include "utils/types.h"

namespace SIMD {
struct HitRecordSIMD {
    Float4 mask; // bool which one > 0
    Float4 t;
};

struct RaySIMD {

    Float4 posX;
    Float4 posY;
    Float4 posZ;

    Float4 dirX;
    Float4 dirY;
    Float4 dirZ;
};

struct SphereSoAsimd {
    Float4 radius;
    Float4 invRadius;

    Float4 positionX;
    Float4 positionY;
    Float4 positionZ;

    Float4 emissionX;
    Float4 emissionY;
    Float4 emissionZ;

    Float4 colorX;
    Float4 colorY;
    Float4 colorZ;

    // Sphere getSphere(int i) {
    //     return Sphere(radius[i], Vec(positionX[i], positionY[i], positionZ[i]),
    //                   Vec(emissionX[i], emissionY[i], emissionZ[i]), Vec(colorX[i], colorY[i], colorZ[i]));
    // }

    inline static const int count = Float4::Size();
    std::array<MaterialType, 4> material;

    Float4 intersect(const RaySIMD &ray, HitRecordSIMD &outHit) const {
        Float4 ocX = positionX - ray.posX;
        Float4 ocY = positionY - ray.posY;
        Float4 ocZ = positionZ - ray.posZ;

        Float4 b = ocX * ray.dirX + ocY * ray.dirY + ocZ * ray.dirZ;
        Float4 c = ocX * ocX + ocY * ocY + ocZ * ocZ - radius * radius;

        Float4 discr = b * b - c;
        Float4 discrPos = discr > Float4::Min();

        if (discrPos.any()) {
            Float4 discrSq = sqrtf(discr);

            Float4 t0 = b - discrSq;
            Float4 t1 = b + discrSq;

            Float4 t = select(t1, t0, t0 > Float4::Min());
            bool4 mask = discrPos & (t > Float4::Min());
            outHit.mask = mask;
            outHit.t = t;
            return t;
        }

        return Float4(-1);
    }
};

bool intersectSoA_SIMD(const Ray &r, Float &t, int &hit_object, const std::vector<SphereSoAsimd> &spheres) {
    using SIMD::Float4;
    HitRecordSIMD outHit{};
    Float4 minDis = Float4::Max();
    RaySIMD ray = {};
    ray.posX = Float4(r.origin.x);
    ray.posY = Float4(r.origin.y);
    ray.posZ = Float4(r.origin.z);

    ray.dirX = Float4(r.dir.x);
    ray.dirY = Float4(r.dir.y);
    ray.dirZ = Float4(r.dir.z);

    __m128i id = _mm_set1_epi32(-1);
    __m128i curId = _mm_set_epi32(3, 2, 1, 0);

    for (auto item : spheres) {
        auto cur_dis = item.intersect(ray, outHit);
        auto mask = outHit.mask & (cur_dis < minDis) & (cur_dis > Float4::Min());
        minDis = select(minDis, cur_dis, mask);
        id = select(id, curId, mask);

        curId = _mm_add_epi32(curId, _mm_set1_epi32(Float4::Size()));
    }

    // now we have up to 4 hits, find and return closest one
    float minDisT = hmin(minDis);
    if (minDisT < INF && minDisT > 1e-4) // any actual hits?
    {
        int minMask = (minDis == Float4(minDisT)).mask();
        if (minMask != 0) {
            int id_scalar[4];
            float hitT_scalar[4];

            _mm_storeu_si128((__m128i *)id_scalar, id);
            _mm_storeu_ps(hitT_scalar, minDis.data);

            // In general, you would do this with a bit scan (first set/trailing zero count).
            // But who cares, it's only 16 options.
            static const int laneId[16] =
                {
                    0, 0, 1, 0, // 00xx
                    2, 0, 1, 0, // 01xx
                    3, 0, 1, 0, // 10xx
                    2, 0, 1, 0, // 11xx
                };

            int lane = laneId[minMask];

            // result
            hit_object = id_scalar[lane];
            t = hitT_scalar[lane];
            spdlog::debug("min_dis:{}, t:{}, hit_object:{}", minDisT, t, hit_object);
            return true;
        }
        spdlog::debug("min_dis:{}, t:{}, hit_object:{}", minDisT, t, hit_object);
    }
    return false;
}
} // namespace SIMD
