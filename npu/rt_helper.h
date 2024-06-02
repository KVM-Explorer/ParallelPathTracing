#pragma once
#include "types.h"
struct Sphere {
    FF rad;
    Vec p, e, c;
    MaterialType m;
    __ai_host__ __ai_core__ Sphere(FF rad_, Vec p_, Vec e_, Vec c_,
                                   MaterialType m_)
        : rad(rad_), p(p_), e(e_), c(c_), m(m_) {}
    __ai_host__ __ai_core__ FF intersect(const Ray &r) const {
        Vec op = p - r.o;
        FF t, epsilon = 1e-4, b = op.dot(r.d),
              det = b * b - op.dot(op) + rad * rad;
        if (det < FF(0))
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > epsilon ? t
                                       : ((t = b + det) > epsilon ? t : FF(0));
    }
};

static Sphere spheres[] = {
    Sphere(1e5, Vec(1e5 + 1, 40.8, 81.6), Vec(), Vec(.75, .25, .25), DIFF),
    Sphere(1e5, Vec(-1e5 + 99, 40.8, 81.6), Vec(), Vec(.25, .25, .75), DIFF),
    Sphere(1e5, Vec(50, 40.8, 1e5), Vec(), Vec(.75, .75, .75), DIFF),
    Sphere(1e5, Vec(50, 40.8, -1e5 + 170), Vec(), Vec(), DIFF),
    Sphere(1e5, Vec(50, 1e5, 81.6), Vec(), Vec(.75, .75, .75), DIFF),
    Sphere(1e5, Vec(50, -1e5 + 81.6, 81.6), Vec(), Vec(.75, .75, .75), DIFF),
    Sphere(16.5, Vec(27, 16.5, 47), Vec(), Vec(1, 1, 1) * .999, SPEC),
    Sphere(16.5, Vec(73, 16.5, 78), Vec(), Vec(1, 1, 1) * .999, REFR),
    Sphere(600, Vec(50, 681.6 - .27, 81.6), Vec(12, 12, 12), Vec(), DIFF)};

constexpr int32_t num_spheres = sizeof(spheres) / sizeof(Sphere);

__ai_host__ __ai_core__ inline FF clamp(FF x) {
    return x < FF(0) ? FF(0) : x > FF(1) ? FF(1) : x;
}
__ai_host__ __ai_core__ inline int toInt(FF x) {
    return int(pow(clamp(x), FF(1 / 2.2)) * 255 + .5);
}

__ai_core__ inline bool intersect(const int num_spheres, const Sphere *spheres,
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
    Vec cl(0, 0, 0), cf(1, 1, 1);
    while (1) {
        if (!intersect(num_spheres, spheres, r, t, id))
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

__ai_host__ __aicore__ static void printVec(const std::string name,
                                            const Vec &v) {
    printf("%s : (%.3f, %.3f, %.3f)\n", name.c_str(), v.x, v.y, v.z);
}