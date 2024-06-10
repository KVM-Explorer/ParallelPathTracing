#pragma once
#include "types.h"
#include <array>
namespace SIMD {

struct RaySSE {
    using Vec = SIMD::VecSSE;
    Vec origin;
    Vec dir;

    RaySSE(Vec origin, Vec direction) : origin(origin), dir(direction) {}
};

// SSE
struct RayPack4 {
    Float4 originX;
    Float4 originY;
    Float4 originZ;

    Float4 dirX;
    Float4 dirY;
    Float4 dirZ;

    RayPack4(RaySSE ray1, RaySSE ray2, RaySSE ray3, RaySSE ray4) {
        originX = {ray1.origin[0], ray2.origin[0], ray3.origin[0], ray4.origin[0]};
        originY = {ray1.origin[1], ray2.origin[1], ray3.origin[1], ray4.origin[1]};
        originZ = {ray1.origin[2], ray2.origin[2], ray3.origin[2], ray4.origin[2]};

        dirX = {ray1.dir[0], ray2.dir[0], ray3.dir[0], ray4.dir[0]};
        dirY = {ray1.dir[1], ray2.dir[1], ray3.dir[1], ray4.dir[1]};
        dirZ = {ray1.dir[2], ray2.dir[2], ray3.dir[2], ray4.dir[2]};
    }
};

struct Sphere {
    using Vec = SIMD::VecSSE;
    Float radius;
    Vec position;
    Vec emission;
    Vec color;
    MaterialType material;

    Sphere(Float radius, Vec position, Vec emission, Vec color, MaterialType material)
        : radius(radius), position(position), emission(emission), color(color), material(material) {}

    Float intersect(const SIMD::RaySSE &ray) const {
        Vec op = position - ray.origin;
        Float t, eps = 1e-4;
        Float b = op.dot(ray.dir);
        Float det = b * b - op.dot(op) + radius * radius;
        if (det < 0)
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }

    Float intersectSIMD(const SIMD::RaySSE &r) const {
        auto pos = position.get();
        auto dir = r.dir.get();
        auto origin = r.origin.get();
        __m128 pos4 = _mm_set_ps(pos.x, pos.y, pos.z, 0.0f);
        __m128 ray_orig4 = _mm_set_ps(origin.x, origin.y, origin.z, 0.0f);
        __m128 ray_dir4 = _mm_set_ps(dir.x, dir.y, dir.z, 0.0f);
        __m128 radius4 = _mm_set1_ps(radius);
        __m128 eps4 = _mm_set1_ps(1e-4f);
        __m128 op4 = _mm_sub_ps(pos4, ray_orig4);   // 计算 op
        __m128 b4 = _mm_dp_ps(op4, ray_dir4, 0x7F); // 计算 b
        __m128 op_dot_op4 = _mm_dp_ps(op4, op4, 0x7F);
        __m128 det4 = _mm_sub_ps(_mm_mul_ps(b4, b4), _mm_sub_ps(op_dot_op4, _mm_mul_ps(radius4, radius4)));
        if (_mm_movemask_ps(_mm_cmplt_ps(det4, _mm_setzero_ps())) == 0x0F)
            return 0.0f;
        __m128 sqrt_det4 = _mm_sqrt_ps(det4);
        __m128 t4 = _mm_sub_ps(b4, sqrt_det4);
        __m128 cmp1 = _mm_cmpgt_ps(t4, eps4);
        t4 = _mm_blendv_ps(_mm_add_ps(b4, sqrt_det4), t4, cmp1);

        return _mm_cvtss_f32(t4);
    }
};

struct SpherePack4 {
    Float4 positionX;
    Float4 positionY;
    Float4 positionZ;
    Float4 radius;

    Float4 emissionX;
    Float4 emissionY;
    Float4 emissionZ;

    Float4 colorX;
    Float4 colorY;
    Float4 colorZ;

    std::array<MaterialType, 4> materials;

    SpherePack4(std::array<Sphere, 4> spheres) {
        positionX = {spheres[0].position[0], spheres[1].position[0], spheres[2].position[0], spheres[3].position[0]};
        positionY = {spheres[0].position[1], spheres[1].position[1], spheres[2].position[1], spheres[3].position[1]};
        positionZ = {spheres[0].position[2], spheres[1].position[2], spheres[2].position[2], spheres[3].position[2]};

        radius = {spheres[0].radius, spheres[1].radius, spheres[2].radius, spheres[3].radius};

        emissionX = {spheres[0].emission[0], spheres[1].emission[0], spheres[2].emission[0], spheres[3].emission[0]};
        emissionY = {spheres[0].emission[1], spheres[1].emission[1], spheres[2].emission[1], spheres[3].emission[1]};
        emissionZ = {spheres[0].emission[2], spheres[1].emission[2], spheres[2].emission[2], spheres[3].emission[2]};

        colorX = {spheres[0].color[0], spheres[1].color[0], spheres[2].color[0], spheres[3].color[0]};
        colorY = {spheres[0].color[1], spheres[1].color[1], spheres[2].color[1], spheres[3].color[1]};
        colorZ = {spheres[0].color[2], spheres[1].color[2], spheres[2].color[2], spheres[3].color[2]};

        materials = {spheres[0].material, spheres[1].material, spheres[2].material, spheres[3].material};
    }

    Float4 intersect(const RayPack4 &ray) const {
        Float4 eps4 = {1e-4f, 1e-4f, 1e-4f, 1e-4f};
        Float4 zero4 = {0.0f, 0.0f, 0.0f, 0.0f};
        Float4 b4X = positionX - ray.originX;
        Float4 b4Y = positionY - ray.originY;
        Float4 b4Z = positionZ - ray.originZ;
        Float4 b4 = b4X.mult(ray.dirX) + b4Y.mult(ray.dirY) + b4Z.mult(ray.dirZ); //

        // TODO: 待实现
        return zero4;
    }
};

struct Camera {
    using Vec = SIMD::VecSSE;
    Vec position;
    Vec direction;
    Vec cx, cy;

    Camera(Vec position, Vec direction, int w, int h) : position(position), direction(direction) {
        cx = Vec(w * .5135 / h);
        cy = (cx % direction).norm() * .5135;
    }
};

} // namespace SIMD
