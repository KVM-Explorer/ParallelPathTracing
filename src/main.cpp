#include "camera.h"
#include "image.h"
#include "parallel/simd/rt_sse.h"
#include "profile.h"
#include "raytracer.h"
#include "rt_cpu.h"
#include "scene.h"
#include <iostream>


enum class Mode {
    CPU,
    SIMD_SSE,
    SIMD_AVX,
    SIMD_AVX_512,
    RawThread,
    OpenMP,
    Async,
    ThreadPool_TBB_QUEUE,
};

int main(int argc, char *argv[]) {

    // configuration

    Image image("image.ppm", 1024, 768);
    Mode mode = Mode::SIMD_SSE;
    Camera cam = Camera(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm(), image);
    Scene scene = CornellBox();
    int samples = 1;
    bool profile = true;

    // render
    std::unique_ptr<RayTracer> raytracer;
    switch (mode) {
    case Mode::CPU: {
        raytracer = std::make_unique<RtCpu>(image, cam, scene, samples);
        break;
    }
    case Mode::SIMD_SSE: {
        raytracer = std::make_unique<RtOptimzationSSE>(image, cam, scene, samples);
        break;
    }

    default:
        raytracer = std::make_unique<RtCpu>(image, cam, scene, samples);
    }

    if (profile) {
        int counts = image.width * image.height * samples * 4;
        Profile Instance([&]() {
            raytracer->render();
        },"RayTracer CPU", counts);
        Instance.run();
        Instance.print();
    } else {
        raytracer->render();
    }

    std::cout << "Image saved to image.ppm" << std::endl;
    // output
    image.save();
}