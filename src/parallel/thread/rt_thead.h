#include "image.h"
#include "raytracer.h"
#include "task_queue.h"
#include "viz_helper.h"
#include <array>

class RtThread : public RayTracer {
  private:
    Image &image;
    Camera camera;
    Scene &scene;
    int samples;
    TaskQueue taskQueue;

  public:
    RtThread(Image &image, Camera camera, Scene &scene, int samples, int numThreads = 12)
        : image(image), camera(camera), scene(scene), samples(samples), taskQueue(numThreads) {}

    void render() override {

        int count = 0;
        for (int y = 0; y < image.height; y++) {                                         // Loop over image rows
            OutputStatus("RayTracer Raw Thread", y, image.height, samples, image.width); // Output status

            for (int x = 0; x < image.width; x++) // Loop cols
            {
                for (int suby = 0; suby < 2; suby++) { // 2x2 subpixel rows
                    auto f = [&, x = x, y = y, suby = suby]() {
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
                    };
                    taskQueue.add(f);
                }
            }
        }
        taskQueue.joinAll();
        std::cout << "\n";
        std::cout << "Total rays: " << count << std::endl;
    }

    Vec testPixel(const Ray &r, Scene &Scene) override {
        return Vec();
    }

    Vec tracing(const Ray &ray, int depth, Scene &scene) {

        bool hit_flag = true;
        std::array<HitRecord, MaxDepth> HitInfo{};

        Ray cur_r = ray;

        for (int i = 0; i < MaxDepth; i++) {
            Float min_dis;
            int hit_object = 0;
            // std::array<HitRecord,MaxDepth>
            if (!intersect(cur_r, min_dis, hit_object, scene)) {
                hit_flag = false;
                break;
            }

            // hit object
            const Sphere &object = scene[hit_object];
            Vec x = cur_r.origin + cur_r.dir * min_dis;
            Vec n = (x - object.position).norm();
            Vec nl = n.dot(cur_r.dir) < 0 ? n : n * -1; // 交点的法线方向，如果是从内部射入物体，则取反
            Vec f = object.color;
            Float p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y
                                                               : f.z;

            // printf("x: (%lf,%lf,%lf), p: %lf t: %d id: %d\n", x.x, x.y, x.z, p,min_dis,hit_object);
            if (++depth > 5) {
                if (random() < p)
                    f = f * (1 / p);
                else {
                    HitInfo[depth - 1] = HitRecord(object.emission);
                    break;
                }
            }

            // limit the depth of the recursion
            if (depth > MaxDepth) {
                HitInfo[depth - 1] = HitRecord(object.emission);
                break;
            }

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
                HitInfo[depth - 1] = HitRecord(object.emission, object.color);
                cur_r = Ray(x, d);
                continue;
            }

            // Specular
            if (object.material == MaterialType::SPECULAR) {
                HitInfo[depth - 1] = HitRecord(object.emission, object.color);
                cur_r = Ray(x, cur_r.dir - n * 2 * n.dot(cur_r.dir));
                continue;
            }

            // Refractive
            Ray reflRay(x, cur_r.dir - n * 2 * n.dot(cur_r.dir));
            bool into = n.dot(nl) > 0;
            Float nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = cur_r.dir.dot(nl), cos2t;
            // printf("ddn: %lf\n", ddn);
            if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) {
                HitInfo[depth - 1] = HitRecord(object.emission, object.color);
                cur_r = reflRay;
                continue;
            }
            Vec tdir = (cur_r.dir * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
            Float a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
            Float Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

            // printf("Re: %lf, Tr: %lf, P: %lf, RP: %lf, TP: %lf\n", Re, Tr, P, RP, TP);

            

            return object.emission + object.color.mult(depth > 2 ? (random() < P ? tracing(reflRay, depth, scene) * RP : tracing(Ray(x, tdir), depth, scene) * TP) : tracing(reflRay, depth, scene) * Re + tracing(Ray(x, tdir), depth, scene) * Tr);
        }

        if (!hit_flag)
            return Vec();

        Vec ret{};
        for (int d = depth - 1; d >= 0; d--) {
            ret = ret.mult(HitInfo[d].diffuse);
            ret = ret + HitInfo[d].emission;
        }
        return ret;
    }
};