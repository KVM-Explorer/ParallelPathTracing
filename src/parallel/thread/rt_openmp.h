#include "image.h"
#include "raytracer.h"
#include "viz_helper.h"

class RtOpenMP : public RayTracer {
  private:
    Image &image;
    Camera camera;
    Scene &scene;
    int samples;

  public:
    RtOpenMP(Image &image, Camera camera, Scene &scene, int samples)
        : image(image), camera(camera), scene(scene), samples(samples) {}

    void render() override {

        int count = 0;
        #pragma omp parallel for schedule(dynamic, 1) private(r) // OpenMP

        for (int y = 0; y < image.height; y++) {                                  // Loop over image rows
            OutputStatus("RayTracer CPU", y, image.height, samples, image.width); // Output status

            for (int x = 0; x < image.width; x++) // Loop cols
            {
                for (int suby = 0; suby < 2; suby++) { // 2x2 subpixel rows
                    Vec ret = Vec();

                    for (int subx = 0; subx < 2; subx++) {
                        for (int s = 0; s < samples; s++) {
                            count++;
                            Float r1 = 2 * random();
                            Float r2 = 2 * random();

                            Float dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                            Float dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                            Vec d = camera.cx * (((subx + .5 + dx) / 2 + x) / image.width - .5) +
                                    camera.cy * (((suby + .5 + dy) / 2 + y) / image.height - .5) + camera.direction;

                            ret = ret + tracing(Ray(camera.position + d * 140, d.norm()), 0, scene);
                        }
                    }
                    image.write(x, image.height - y - 1, Vec(clamp(ret.x), clamp(ret.y), clamp(ret.z)) * .25);
                }
            }
        }
        std::cout << "\n";
        std::cout << "Total rays: " << count << std::endl;
    }

    Vec testPixel(const Ray &r, Scene &Scene) override {
        return Vec();
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
        // printf("ddn: %lf\n", ddn);
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0)
            return object.emission + object.color.mult(tracing(reflRay, depth, scene));
        Vec tdir = (r.dir * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
        Float a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
        Float Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

        // printf("Re: %lf, Tr: %lf, P: %lf, RP: %lf, TP: %lf\n", Re, Tr, P, RP, TP);

        return object.emission + object.color.mult(depth > 2 ? (random() < P ? tracing(reflRay, depth, scene) * RP : tracing(Ray(x, tdir), depth, scene) * TP) : tracing(reflRay, depth, scene) * Re + tracing(Ray(x, tdir), depth, scene) * Tr);
    }
};