#include "scene.h"
#include "utils/types.h"
namespace SIMD {

template <typename T>
struct SphereSoAsimd {
    T radius;

    T positionX;
    T positionY;
    T positionZ;

    T emissionX;
    T emissionY;
    T emissionZ;

    T colorX;
    T colorY;
    T colorZ;

    Sphere getSphere(int i) {
        return Sphere(radius[i], Vec(positionX[i], positionY[i], positionZ[i]),
                      Vec(emissionX[i], emissionY[i], emissionZ[i]), Vec(colorX[i], colorY[i], colorZ[i]));
    }

    static constexpr int count = T::size();
    std::array<MaterialType, count> material;

    Float intersect(const Ray &ray, HitRecord &outHit) const {
        T hitT = T(INF);

        T rayX = T(ray.dir.x);
        T rayY = T(ray.dir.y);
        T rayZ = T(ray.dir.z);

        for (int i = 0; i < count; i += T::size()) {
            T opX = positionX[i] - rayX;
            T opY = positionY[i] - rayY;
            T opZ = positionZ[i] - rayZ;

            T b = opX * rayX + opY * rayY + opZ * rayZ;
            T det = b * b - (opX * opX + opY * opY + opZ * opZ) + radius[i] * radius[i];

            T mask = det < 0;
            det = sqrt(det);

            T t1 = b - det;
            T t2 = b + det;

            T t = t1 > 1e-4 ? t1 : (t2 > 1e-4 ? t2 : 0);

            hitT = select(t < hitT, t, hitT);
        }

        // TODO: Select & record the hit
    }
};

void intersectSoA_SIMD(const Ray &r, Float &t, int &hit_object, const Sphere &spheres) {
    // HitRecord outHit{};
    // auto min_dis = spheres.intersect(r, outHit);
    // if (min_dis >= 0) {
    //     t = min_dis;
    //     hit_object = outHit.id;
    // }
}
} // namespace SIMD
