#pragma once
#include "debug_helper.h"
#include "rt_math.h"
#include "scene.h"
#include "utils/rt_helper.h"
#include "utils/types.h"

struct SphereSoA {
    SphereSoA(int count) {
        radius = new Float[count];
        invRadius = new Float[count];

        positionX = new Float[count];
        positionY = new Float[count];
        positionZ = new Float[count];

        emissionX = new Float[count];
        emissionY = new Float[count];
        emissionZ = new Float[count];

        colorX = new Float[count];
        colorY = new Float[count];
        colorZ = new Float[count];

        material = new MaterialType[count];

        this->count = count;
    }

    ~SphereSoA() {
        delete[] radius;
        delete[] invRadius;

        delete[] positionX;
        delete[] positionY;
        delete[] positionZ;

        delete[] emissionX;
        delete[] emissionY;
        delete[] emissionZ;

        delete[] colorX;
        delete[] colorY;
        delete[] colorZ;

        delete[] material;
    }

    Float *radius;
    Float *invRadius;

    Float *positionX;
    Float *positionY;
    Float *positionZ;

    Float *emissionX;
    Float *emissionY;
    Float *emissionZ;

    Float *colorX;
    Float *colorY;
    Float *colorZ;

    MaterialType *material;

    int count;

    Sphere getSphere(int i) const {
        if (i != 0) {
            int u = 0;
        }
        return Sphere(radius[i], Vec(positionX[i], positionY[i], positionZ[i]), Vec(emissionX[i], emissionY[i], emissionZ[i]),
                      Vec(colorX[i], colorY[i], colorZ[i]), material[i]);
    }

    Float intersect(const Ray &ray, HitRecord &outHit) const {
        float hitT = INF;
        Float eps = 1e-4;
        int id = -1;
        for (int i = 0; i < count; ++i) {
            float coX = positionX[i] - ray.origin.x;
            float coY = positionY[i] - ray.origin.y;
            float coZ = positionZ[i] - ray.origin.z;
            float nb = coX * ray.dir.x + coY * ray.dir.y + coZ * ray.dir.z; // op.dot(ray.dir)
            float c = coX * coX + coY * coY + coZ * coZ - radius[i] * radius[i];
            float discr = nb * nb - c;
            if (discr > 0) {
                float discrSq = sqrt(discr);

                // Try earlier t
                float t = nb - discrSq;
                if (t <= eps) // before min, try later t!
                    t = nb + discrSq;

                if (t > eps && t < hitT) {
                    id = i;
                    hitT = t;
                }
            }
        }
        if (id != -1) {
            outHit.pos = ray.at(hitT);
            outHit.normal = (outHit.pos - Vec(positionX[id], positionY[id], positionZ[id])) * invRadius[id];
            outHit.t = hitT;
            outHit.id = id;
            return hitT;
        } else
            return -1;
    }
};

bool static intersectSoA(const Ray &r, Float &t, int &hit_object, const SphereSoA *spheres) {
    HitRecord outHit{};
    auto min_dis = spheres->intersect(r, outHit);

    if (min_dis >= 0) {
        t = min_dis;
        hit_object = outHit.id;
        spdlog::debug("min_dis: {}, t: {}, hit_object: {}", min_dis, t, hit_object);
        return true;
    }
    spdlog::debug("min_dis: {}, t: {}, hit_object: {}", min_dis, t, hit_object);
    return false;
}