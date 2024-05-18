#include "image.h"
#include "raytracer.h"
#include "task_queue.h"
#include "viz_helper.h"
#include <stack>
#include <tuple>

class RtThread : public RayTracer {
  private:
    Image &image;
    Camera camera;
    Scene &scene;
    int samples;
    TaskQueue taskQueue;

  public:
    RtThread(Image &image, Camera camera, Scene &scene, int samples, int numThreads = 1)
        : image(image), camera(camera), scene(scene), samples(samples), taskQueue(numThreads) {}

    void render() override {

        int count = 0;
        for (int y = 0; y < image.height; y++) {                                         // Loop over image rows
            OutputStatus("RayTracer Raw Thread", y, image.height, samples, image.width); // Output status

            for (int x = 0; x < image.width; x++) // Loop cols
            {
                auto func = [&, y = y, x = x]() {
                    // printf("%d %d\n",x,y);
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
                                // printf("1");
                            }
                        }
                        // image.write(x, image.height - y - 1, Vec(clamp(ret.x), clamp(ret.y), clamp(ret.z)) * .25);
                    }
                };
                taskQueue.add(func);
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

        std::stack<std::tuple<Ray, int>> traceStack;
        traceStack.push(std::make_tuple(ray, depth));

        Vec result = Vec();

        while (!traceStack.empty()) {
            Ray currentRay = std::get<0>(traceStack.top());
            int currentDepth = std::get<1>(traceStack.top());
            traceStack.pop();

            Float min_dis;
            int hit_object = 0;

            if (!intersect(currentRay, min_dis, hit_object, scene))
                continue;

            const Sphere &object = scene[hit_object];
            Vec x = currentRay.origin + currentRay.dir * min_dis;
            Vec n = (x - object.position).norm();
            Vec nl = n.dot(currentRay.dir) < 0 ? n : n * -1;
            Vec f = object.color;
            Float p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y
                                                               : f.z;

            if (++currentDepth > 5) {
                if (random() < p)
                    f = f * (1 / p);
                else
                    result = object.emission;
                continue;
            }

            if (currentDepth > MaxDepth) {
                result = object.emission;
                continue;
            }

            if (object.material == MaterialType::DIFFUSE) {
                Float r1 = 2 * PI * random();
                Float r2 = random();
                Float r2s = sqrt(r2);
                Vec w = nl;
                Vec u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)) % w).norm();
                Vec v = w % u;
                Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();
                traceStack.push(std::make_tuple(Ray(x, d), currentDepth));
                continue;
            }

            if (object.material == MaterialType::SPECULAR) {
                traceStack.push(std::make_tuple(Ray(x, currentRay.dir - n * 2 * n.dot(currentRay.dir)), currentDepth));
                continue;
            }

            Ray reflRay(x, currentRay.dir - n * 2 * n.dot(currentRay.dir));
            bool into = n.dot(nl) > 0;
            Float nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = currentRay.dir.dot(nl), cos2t;

            if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) {
                traceStack.push(std::make_tuple(reflRay, currentDepth));
                continue;
            }

            Vec tdir = (currentRay.dir * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
            Float a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
            Float Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

            result = object.emission + object.color.mult(depth > 2 ? (random() < P ? tracing(reflRay, currentDepth, scene) * RP : tracing(Ray(x, tdir), currentDepth, scene) * TP) : tracing(reflRay, currentDepth, scene) * Re + tracing(Ray(x, tdir), currentDepth, scene) * Tr);
        }

        return result;
    }
};