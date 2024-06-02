#pragma once
#include "scene.h"
#include "types.h"

__ai_host__ __aicore__ inline FF clamp(FF x) {
    return x < FF(0) ? FF(0) : x > FF(1) ? FF(1) : x;
}
__ai_host__ __aicore__ inline int toInt(FF x) {
    return int(pow(clamp(x), FF(1 / 2.2)) * 255 + .5);
}

__ai_host__ __aicore__ static void printVec(const std::string name,
                                            const Vec &v) {
    printf("%s : (%.5f, %.5f, %.5f)\n", name.c_str(), v.x, v.y, v.z);
}

__aicore__ inline bool intersect(const int num_spheres, const Sphere *spheres,
                                  const Ray &r, float &t, int &id) {
    float n = num_spheres, d, inf = t = 1e20;
    for (int i = int(n); i--;) 
        if ((d = spheres[i].intersect(r)) && d < t) {
            t = d;
            id = i;
        }
    return t < inf;
}

__ai_host__ __aicore__ inline Vec radiance(const Ray &input_r,
                                           int input_depth) {

    float t;
    int id = 0;
    Ray r = input_r;
    int depth = input_depth;
    Vec cl(0, 0, 0); // accumulated color
    Vec cf(1, 1, 1); // accumulated reflectance
    auto spheres = testSphere();
    int num_spheres = spheres.size();
    while (1) {
        if (!intersect(num_spheres, spheres.data(), r, t, id))
            return cl;                   // if miss, return black
        const Sphere &obj = spheres[id]; // the hit object
        Vec x = r.o + r.d * t, n = (x - obj.p).norm(),
            nl = n.dot(r.d) < FF(0) ? n : n * -1, f = obj.c;
        float p = f.x > f.y && f.x > f.z ? f.x
                  : f.y > f.z            ? f.y
                                         : f.z; // max refl
        cl = cl + cf.mult(obj.e);
        if (++depth > 5)

            if (randomFF() < p)
                f = f * (1 / p);
            else
                return cl; // R.R.
        cf = cf.mult(f);
        if (obj.m == DIFF) { // Ideal DIFFUSE reflection
            double r1 = 2 * M_PI * randomFF(), r2 = randomFF(), r2s = sqrt(r2);
            Vec w = nl,
                u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)).cross(w)).norm(),
                v = w.cross(u);
            Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2))
                        .norm();
            // return obj.e + f.mult(radiance(Ray(x,d),depth,Xi));
            r = Ray(x, d);
            continue;
        } else if (obj.m == SPEC) { // Ideal SPECULAR reflection
            // return obj.e +
            // f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,Xi));
            r = Ray(x, r.d - n * 2 * n.dot(r.d));
            continue;
        }
        Ray reflRay(x, r.d - n * 2 * n.dot(r.d)); // Ideal dielectric REFRACTION
        bool into = n.dot(nl) > FF(0);            // Ray fromoutside going in?
        int nc = 1;
        float nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.d.dot(nl),
              cos2t;
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) <
            0) { // Total internal reflection
            // return obj.e + f.mult(radiance(reflRay,depth,Xi));
            r = reflRay;
            continue;
        }
        Vec tdir =
            (r.d * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t))))
                .norm();
        float a = nt - nc, b = nt + nc, R0 = a * a / (b * b),
              c = 1 - (into ? -ddn : tdir.dot(n));
        float Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re,
              P = .25f + .5f * Re, RP = Re / P, TP = Tr / (1 - P);
        // return obj.e + f.mult(erand48(Xi)<P ?
        //                       radiance(reflRay,    depth,Xi)*RP:
        //                       radiance(Ray(x,tdir),depth,Xi)*TP);
        if (random() < P) {
            cf = cf * RP;
            r = reflRay;
        } else {
            cf = cf * TP;
            r = Ray(x, tdir);
        }
        continue;
    }
}

__ai_host__ __aicore__ inline Vec testRadiance(const Ray &input_r,
                                               int input_depth) {
    // return Vec(input_r.d.x, input_r.d.y, input_r.d.z);
    // Vec ret = Vec(input_r.d.x, input_r.d.y, input_r.d.z);
    Vec ret = input_r.d;

    auto spheres = cornelbox();

    ret = spheres[0].p;

    return ret;
}
