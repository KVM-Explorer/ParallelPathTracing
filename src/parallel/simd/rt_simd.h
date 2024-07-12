
#include "camera.h"
#include "parallel/simd/math_simd.h"
#include "parallel/simd/simd_helper.h"
#include "raytracer.h"
#include "rt_math.h"
#include "scene.h"
#include "viz_helper.h"
#include <vector>

class RayTracingSIMD : public RayTracer {
  private:
    Image &image;
    Camera camera;
    std::vector<SIMD::SphereSoAsimd> spheresSIMD;
    Scene sceneSpheres;
    int samples;

  public:
    RayTracingSIMD(Image &image, Camera camera, Scene &scene, int samples)
        : image(image), samples(samples), camera(camera) {
        using SIMD::Float4;
        auto count = scene.size();
        int padding = 4 - count % 4;
        
        for (int i = 0; i < count; i += SIMD::Float4::Size()) {
            SIMD::SphereSoAsimd sphereSIMD;

            sphereSIMD.radius = Float4(scene[i].radius, scene[i + 1].radius, scene[i + 2].radius, scene[i + 3].radius);
            sphereSIMD.invRadius = Float4(1 / scene[i].radius, 1 / scene[i + 1].radius, 1 / scene[i + 2].radius, 1 / scene[i + 3].radius);

            sphereSIMD.positionX = Float4(scene[i].position.x, scene[i + 1].position.x, scene[i + 2].position.x, scene[i + 3].position.x);
            sphereSIMD.positionY = Float4(scene[i].position.y, scene[i + 1].position.y, scene[i + 2].position.y, scene[i + 3].position.y);
            sphereSIMD.positionZ = Float4(scene[i].position.z, scene[i + 1].position.z, scene[i + 2].position.z, scene[i + 3].position.z);

            sphereSIMD.emissionX = Float4(scene[i].emission.x, scene[i + 1].emission.x, scene[i + 2].emission.x, scene[i + 3].emission.x);
            sphereSIMD.emissionY = Float4(scene[i].emission.y, scene[i + 1].emission.y, scene[i + 2].emission.y, scene[i + 3].emission.y);
            sphereSIMD.emissionZ = Float4(scene[i].emission.z, scene[i + 1].emission.z, scene[i + 2].emission.z, scene[i + 3].emission.z);

            sphereSIMD.colorX = Float4(scene[i].color.x, scene[i + 1].color.x, scene[i + 2].color.x, scene[i + 3].color.x);
            sphereSIMD.colorY = Float4(scene[i].color.y, scene[i + 1].color.y, scene[i + 2].color.y, scene[i + 3].color.y);
            sphereSIMD.colorZ = Float4(scene[i].color.z, scene[i + 1].color.z, scene[i + 2].color.z, scene[i + 3].color.z);

            sphereSIMD.material = {scene[i].material, scene[i + 1].material, scene[i + 2].material, scene[i + 3].material};

            spheresSIMD.push_back(sphereSIMD);
        }

        for (int i = 0; i < padding; i++) {
            spheresSIMD.push_back({});
        }

        sceneSpheres = scene;
    }

    std::string name() override {
        return "RayTracer SoA SIMD CPU";
    }

    void render() override {
        int count = 0;
        for (int y = 0; y < image.height; y++) {                         // Loop over image rows
            OutputStatus(name(), y, image.height, samples, image.width); // Output status

            for (int x = 0; x < image.width; x++) // Loop cols
            {
                for (int suby = 0; suby < 2; suby++) { // 2x2 subpixel rows
                    Vec ret = {};
                    for (int subx = 0; subx < 2; subx++) {
                        for (int s = 0; s < samples; s++) {
                            count++;
                            Float r1 = 2 * random();
                            Float r2 = 2 * random();

                            Float dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                            Float dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                            Vec d = camera.cx * (((subx + .5 + dx) / 2 + x) / image.width - .5) +
                                    camera.cy * (((suby + .5 + dy) / 2 + y) / image.height - .5) + camera.direction;

                            ret = ret + tracing(Ray(camera.position + d * 140, d.norm()), 0) * (1. / samples);
                        }
                    }
                    image.write(x, image.height - y - 1, ret * .25);
                }
            }
        }
        std::cout << "\n";
        std::cout << "Total rays: " << count << std::endl;
    }

  private:
  private:
    Vec tracing(const Ray &r, int depth) {

        Float min_dis;
        int hit_object = 0;

        if (!SIMD::intersectSoA_SIMD(r, min_dis, hit_object, spheresSIMD))
            return Vec();
        // hit object
        const Sphere &object = sceneSpheres[hit_object];
        Vec x = r.origin + r.dir * min_dis;
        Vec n = (x - object.position).norm();
        Vec nl = n.dot(r.dir) < 0 ? n : n * -1; // 交点的法线方向，如果是从内部射入物体，则取反
        Vec f = object.color;
        Float p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y
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
            Float r1 = 2 * PI * random();
            Float r2 = random();
            Float r2s = sqrt(r2);
            Vec w = nl;
            Vec u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)) % w).norm();
            Vec v = w % u;
            Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();
            // printf("d: (%lf,%lf,%lf)\n", d.x, d.y, d.z);

            return object.emission + object.color.mult(tracing(Ray(x, d), depth));
        }

        // Specular
        if (object.material == MaterialType::SPECULAR) {
            return object.emission + object.color.mult(tracing(Ray(x, r.dir - n * 2 * n.dot(r.dir)), depth));
        }

        // Refractive
        Ray reflRay(x, r.dir - n * 2 * n.dot(r.dir));
        bool into = n.dot(nl) > 0;
        Float nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.dir.dot(nl), cos2t;
        // printf("ddn: %lf\n", ddn);
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0)
            return object.emission + object.color.mult(tracing(reflRay, depth));
        Vec tdir = (r.dir * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
        Float a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
        Float Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

        // printf("Re: %lf, Tr: %lf, P: %lf, RP: %lf, TP: %lf\n", Re, Tr, P, RP, TP);

        return object.emission + object.color.mult(depth > 2 ? (random() < P ? tracing(reflRay, depth) * RP : tracing(Ray(x, tdir), depth) * TP) : tracing(reflRay, depth) * Re + tracing(Ray(x, tdir), depth) * Tr);
    }
};