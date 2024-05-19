#include "image.h"
#include "raytracer.h"
#include "viz_helper.h"
#include <omp.h>

enum class OpenMPMode {
    ParallelFor,
    ParallelTask,
    ParallelWorkStealing,
};

class RtOpenMP : public RayTracer {
  private:
    Image &image;
    Camera camera;
    Scene &scene;
    int samples;
    OpenMPMode mode;

  public:
    RtOpenMP(Image &image, Camera camera, Scene &scene, int samples, OpenMPMode mode)
        : image(image), camera(camera), scene(scene), samples(samples), mode(mode) {}

    void render() override {

        int count = 0;
        switch (mode) {
        case OpenMPMode::ParallelFor:
            openmpParallelFor();
            break;
        case OpenMPMode::ParallelTask:
            openmpTask();
            break;
        case OpenMPMode::ParallelWorkStealing:
            openmpWorkStealing();
            break;
        }

        std::cout << "\n";
        std::cout << "Total rays: " << count << std::endl;
    }

    std::string name() override {
        return "RayTracer OpenMP";
    }

    Vec testPixel(const Ray &r, Scene &Scene) override {
        return Vec();
    }

    void openmpParallelFor() {
        // clang-format off
        #pragma omp parallel for
        for (int y = 0; y < image.height; y++) {
            for (int x = 0; x < image.width; x++) {
                for (int suby = 0; suby < 2; suby++) { // 2x2 subpixel rows
                    Vec ret = Vec();

                    for (int subx = 0; subx < 2; subx++) {
                        for (int s = 0; s < samples; s++) {
                            Float r1 = 2 * random();
                            Float r2 = 2 * random();

                            Float dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                            Float dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                            Vec d = camera.cx * (((subx + .5 + dx) / 2 + x) / image.width - .5) +
                                    camera.cy * (((suby + .5 + dy) / 2 + y) / image.height - .5) + camera.direction;

                            Vec color = tracing(Ray(camera.position + d * 140, d.norm()), 0, scene) * (1. / samples);
                            ;
                            #pragma omp atomic
                            ret = ret + color;
                        }
                    }
                    #pragma omp atomic
                    image.write(x, image.height - y - 1, Vec(clamp(ret.x), clamp(ret.y), clamp(ret.z)) * .25);
                }
            }
        }
        // clang-format on
    }

    void openmpTask() {
        // clang-format off

        #pragma omp parallel
        {
            // #pragma omp single
            #pragma omp for schedule(dynamic, taskSize)   // tasksize   
            {
                for (int y = 0; y < image.height; y++) {
                    #pragma omp task
                    {
                        for (int x = 0; x < image.width; x++) {
                            for (int suby = 0; suby < 2; suby++) { // 2x2 subpixel rows
                                Vec ret = Vec();

                                for (int subx = 0; subx < 2; subx++) {
                                    for (int s = 0; s < samples; s++) {
                                        Float r1 = 2 * random();
                                        Float r2 = 2 * random();

                                        Float dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                                        Float dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                                        Vec d = camera.cx * (((subx + .5 + dx) / 2 + x) / image.width - .5) +
                                                camera.cy * (((suby + .5 + dy) / 2 + y) / image.height - .5) + camera.direction;

                                        Vec color = tracing(Ray(camera.position + d * 140, d.norm()), 0, scene) * (1. / samples);
                                        #pragma omp atomic
                                        ret = ret + color;
                                    }
                                }
                                #pragma omp atomic
                                image.write(x, image.height - y - 1, Vec(clamp(ret.x), clamp(ret.y), clamp(ret.z)) * .25);
                            }
                        }
                    }
                }
            }
        }
        // clang-format on
    }

    void openmpWorkStealing() {
        // clang-format off
        #pragma omp parallel
        {
            // #pragma omp single
            #pragma omp for schedule(guided)   // work stealing 
            {
                for (int y = 0; y < image.height; y++) {
                    #pragma omp task
                    {
                        for (int x = 0; x < image.width; x++) {
                            for (int suby = 0; suby < 2; suby++) { // 2x2 subpixel rows
                                Vec ret = Vec();

                                for (int subx = 0; subx < 2; subx++) {
                                    for (int s = 0; s < samples; s++) {
                                        Float r1 = 2 * random();
                                        Float r2 = 2 * random();

                                        Float dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                                        Float dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                                        Vec d = camera.cx * (((subx + .5 + dx) / 2 + x) / image.width - .5) +
                                                camera.cy * (((suby + .5 + dy) / 2 + y) / image.height - .5) + camera.direction;

                                        Vec color = tracing(Ray(camera.position + d * 140, d.norm()), 0, scene) * (1. / samples);
                                        #pragma omp atomic
                                        ret = ret + color;
                                    }
                                }
                                #pragma omp atomic
                                image.write(x, image.height - y - 1, Vec(clamp(ret.x), clamp(ret.y), clamp(ret.z)) * .25);
                            }
                        }
                    }
                }
            }
        }
        // clang-format on
    }

    Vec tracing(const Ray &r, int depth, Scene &scene) {

        Float min_dis;
        int hit_object = 0;

        if (!intersect(r, min_dis, hit_object, scene))
            return Vec();
        // hit object
        const Sphere &object = scene[hit_object];
        Vec x = r.origin + r.dir * min_dis;
        Vec n = (x - object.position).norm();
        Vec nl = n.dot(r.dir) < 0 ? n : n * -1; // 交点的法线方向，如果是从内部射入物体，则取反
        Vec f = object.color;
        Float p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y
                                                           : f.z;

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
            Float r1 = 2 * PI * random();
            Float r2 = random();
            Float r2s = sqrt(r2);
            Vec w = nl;
            Vec u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)) % w).norm();
            Vec v = w % u;
            Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();

            return object.emission + object.color.mult(tracing(Ray(x, d), depth, scene));
        }

        // Specular
        if (object.material == MaterialType::SPECULAR) {
            return object.emission + object.color.mult(tracing(Ray(x, r.dir - n * 2 * n.dot(r.dir)), depth, scene));
        }

        // Refractive
        Ray reflRay(x, r.dir - n * 2 * n.dot(r.dir));
        bool into = n.dot(nl) > 0;
        Float nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.dir.dot(nl), cos2t;
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0)
            return object.emission + object.color.mult(tracing(reflRay, depth, scene));
        Vec tdir = (r.dir * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
        Float a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
        Float Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

        return object.emission + object.color.mult(depth > 2 ? (random() < P ? tracing(reflRay, depth, scene) * RP : tracing(Ray(x, tdir), depth, scene) * TP) : tracing(reflRay, depth, scene) * Re + tracing(Ray(x, tdir), depth, scene) * Tr);
    }
};