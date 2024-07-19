#pragma once
#include "debug_helper.h"
#include "rt_math.h"
#include "utils/rt_helper.h"
#include <vector>

using Scene = std::vector<Sphere>;

struct HitRecord {
    Vec pos;
    Vec normal;
    Float t;
    int id;
};

// 判断是否命中物体
inline bool intersect(const Ray &r, Float &t, int &hit_object, Scene &scene) {

    Float min_dis = t = INF;

    for (int i = 0; i < scene.size(); i++) {

        min_dis = scene[i].intersect(r);
        if (min_dis && min_dis < t) {
            t = min_dis;
            hit_object = i;
        }
    }

    spdlog::debug("min_dis: {}, t: {}, hit_object: {}", min_dis, t, hit_object);

    return t < INF;
}

Scene CornellBox() {
    return {
        Sphere(1e5, Vec(1e5 + 1, 40.8, 81.6), Vec(), Vec(.75, .25, .25), MaterialType::DIFFUSE),   // Left
        Sphere(1e5, Vec(-1e5 + 99, 40.8, 81.6), Vec(), Vec(.25, .25, .75), MaterialType::DIFFUSE), // Rght
        Sphere(1e5, Vec(50, 40.8, 1e5), Vec(), Vec(.75, .75, .75), MaterialType::DIFFUSE),         // Back
        Sphere(1e5, Vec(50, 40.8, -1e5 + 170), Vec(), Vec(), MaterialType::DIFFUSE),               // Frnt
        Sphere(1e5, Vec(50, 1e5, 81.6), Vec(), Vec(.75, .75, .75), MaterialType::DIFFUSE),         // Botm
        Sphere(1e5, Vec(50, -1e5 + 81.6, 81.6), Vec(), Vec(.75, .75, .75), MaterialType::DIFFUSE), // Top
        Sphere(16.5, Vec(27, 16.5, 47), Vec(), Vec(1, 1, 1) * .999, MaterialType::SPECULAR),       // Mirr
        Sphere(16.5, Vec(73, 16.5, 78), Vec(), Vec(1, 1, 1) * .999, MaterialType::REFRACTIVE),     // Glas
        Sphere(600, Vec(50, 681.6 - .27, 81.6), Vec(12, 12, 12), Vec(), MaterialType::DIFFUSE)     // Lite
    };
}

Scene CornelBoxWithPlantySphere(int count) {

    Scene scene{
        Sphere(1e5, Vec(1e5 + 1, 40.8, 81.6), Vec(), Vec(.75, .25, .25), MaterialType::DIFFUSE),   // Left
        Sphere(1e5, Vec(-1e5 + 99, 40.8, 81.6), Vec(), Vec(.25, .25, .75), MaterialType::DIFFUSE), // Rght
        Sphere(1e5, Vec(50, 40.8, 1e5), Vec(), Vec(.75, .75, .75), MaterialType::DIFFUSE),         // Back
        Sphere(1e5, Vec(50, 40.8, -1e5 + 170), Vec(), Vec(), MaterialType::DIFFUSE),               // Frnt
        Sphere(1e5, Vec(50, 1e5, 81.6), Vec(), Vec(.75, .75, .75), MaterialType::DIFFUSE),         // Botm
        Sphere(1e5, Vec(50, -1e5 + 81.6, 81.6), Vec(), Vec(.75, .75, .75), MaterialType::DIFFUSE), // Top
        Sphere(16.5, Vec(27, 16.5, 47), Vec(), Vec(1, 1, 1) * .999, MaterialType::SPECULAR),       // Mirr
        Sphere(16.5, Vec(73, 16.5, 78), Vec(), Vec(1, 1, 1) * .999, MaterialType::REFRACTIVE),     // Glas
        Sphere(600, Vec(50, 681.6 - .27, 81.6), Vec(12, 12, 12), Vec(), MaterialType::DIFFUSE)     // Lite
    };

    for (int i = 0; i < count; i++) {
        Vec center = Vec(random() * 100, random() * 100, random() * 100);
        Vec color = Vec(random(), random(), random());
        Float radius = random() * 10;
        MaterialType material = MaterialType::DIFFUSE;
        scene.push_back(Sphere(radius, center, Vec(), color, material));
    }

    return scene;
}