#pragma once
#include "raytracer.h"
#include "utils/meta.h"

class RtCpu : public RayTracer {
  public:
    RtCpu() {
    }
    void render(Image &image, Camera camera, Scene &scene, int samples) override {

        for (int y = 0; y < image.height; y++) {                                  // Loop over image rows
            OutputStatus("RayTracer CPU", y, image.height, samples, image.width); // Output status

            for (int x = 0; x < image.width; x++) // Loop cols
            {
                for (int suby = 0; suby < 2; suby++) { // 2x2 subpixel rows
                    Vec ret = Vec();

                    for (int subx = 0; subx < 2; subx++) {
                        for (int s = 0; s < samples; s++) {
                            double r1 = 2 * random();
                            double r2 = 2 * random();

                            double dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                            double dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                            Vec d = camera.cx * (((subx + .5 + dx) / 2 + x) / image.width - .5) +
                                    camera.cy * (((suby + .5 + dy) / 2 + y) / image.height - .5) + camera.direction;

                            ret = ret + tracing(Ray(camera.position + d * 140, d.norm()), 0, scene);
                            // printf("x: %d, y: %d, subx: %d, suby: %d, s: %d d:(%lf,%lf,%lf) tracing:(%lf,%lf,%lf)\n", x, y, subx, suby, s, d.x, d.y, d.z, tmp.x, tmp.y, tmp.z);
                        }
                    }
                    image.write(x, image.height - y - 1, Vec(clamp(ret.x), clamp(ret.y), clamp(ret.z)) * .25);
                }
            }
        }
    }

    void testPixel(Scene &Scene) {
        // x: 809, y: 0, subx: 1, suby: 1, s: 0 d:(0.189355,-0.283993,-0.939943) tracing:(0.000000,0.000000,0.000000)

        // int x = 809;
        // int y = 0;
        // int subx = 1;
        // int suby = 1;
        // int s = 0;
        // Vec d = Vec(0.189355, -0.283993, -0.939943);

        // x: 5, y: 0, subx: 0, suby: 1, s: 0 d:(-0.311788,-0.274857,-0.909529) tracing:(0.438120,0.146040,1.314361)

        int x = 5;
        int y = 0;
        int subx = 0;
        int suby = 1;
        int s = 0;
        Vec d = Vec(-0.311788, -0.274857, -0.909529);

        Vec tmp = tracing(Ray(Vec(50, 52, 295.6) + d * 140, d.norm()), 0, Scene);
    }

    Vec tracing(const Ray &r, int depth, Scene &scene) {

        double min_dis;
        int hit_object = 0;

        if (!intersect(r, min_dis, hit_object, scene))
            return Vec();
        // hit object
        const Sphere &object = scene[hit_object];
        Vec x = r.origin + r.dir * min_dis;
        Vec n = (x - object.position).norm();
        Vec nl = n.dot(r.dir) < 0 ? n : n * -1; // 交点的法线方向，如果是从内部射入物体，则取反
        Vec f = object.color;
        double p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y
                                                            : f.z;

        // printf("x: (%lf,%lf,%lf), p: %lf t: %d id: %d\n", x.x, x.y, x.z, p,min_dis,hit_object);
        if (++depth > 5) {
            if (random() < p)
                f = f * (1 / p);
            else
                return object.emission;
        }

        // limit the depth of the recursion
        if (depth > MaxDepth)
            return object.emission;

        //  Diffuse
        if (object.material == MaterialType::DIFFUSE) {
            double r1 = 2 * PI * random();
            double r2 = random();
            double r2s = sqrt(r2);
            Vec w = nl;
            Vec u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)) % w).norm();
            Vec v = w % u;
            Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();
            // printf("d: (%lf,%lf,%lf)\n", d.x, d.y, d.z);

            return object.emission + object.color.mult(tracing(Ray(x, d), depth, scene));
        }

        // Specular
        if (object.material == MaterialType::SPECULAR) {
            return object.emission + object.color.mult(tracing(Ray(x, r.dir - n * 2 * n.dot(r.dir)), depth, scene));
        }

        // Refractive
        Ray reflRay(x, r.dir - n * 2 * n.dot(r.dir));
        bool into = n.dot(nl) > 0;
        double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.dir.dot(nl), cos2t;
        // printf("ddn: %lf\n", ddn);
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0)
            return object.emission + object.color.mult(tracing(reflRay, depth, scene));
        Vec tdir = (r.dir * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
        double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
        double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

        // printf("Re: %lf, Tr: %lf, P: %lf, RP: %lf, TP: %lf\n", Re, Tr, P, RP, TP);

        return object.emission + object.color.mult(depth > 2 ? (random() < P ? tracing(reflRay, depth, scene) * RP : tracing(Ray(x, tdir), depth, scene) * TP) : tracing(reflRay, depth, scene) * Re + tracing(Ray(x, tdir), depth, scene) * Tr);
    }
};