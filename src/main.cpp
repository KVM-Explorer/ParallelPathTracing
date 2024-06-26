#include "camera.h"
#include "image.h"
#include "parallel/simd/rt_sse.h"
#include "parallel/thread/rt_openmp.h"
#include "parallel/thread/rt_thead.h"
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
};

int main(int argc, char *argv[]) {

    // configuration

    Image image("image.ppm", 512, 512);
    Mode mode = Mode::CPU;
    Camera cam = Camera(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm(), image);
    Scene scene = CornellBox();
    int samples = 1; // 2 * 2 * samples per pixel
    bool profile = true;
    int threadNum = 8;
    ThreadLoadType splitThreadLoadType = ThreadLoadType::Row;
    OpenMPMode openmpMode = OpenMPMode::ParallelTask;
    int taskSize = 1 * 1;

    if (argc > 1) {
        // Parse the taskSize argument
        taskSize = std::atoi(argv[1]);
        std::cout << std::format("Input taskSize: {}\n", taskSize);

        // samples = std::atoi(argv[1]);
        // std::cout << std::format("Input samples: {} sample rays: {}\n", samples, samples * 4);
    }

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
    case Mode::RawThread: {
        raytracer = std::make_unique<RtThread>(image, cam, scene, samples, threadNum, taskSize, splitThreadLoadType);
        break;
    }

    case Mode::OpenMP: {
        raytracer = std::make_unique<RtOpenMP>(image, cam, scene, samples,openmpMode,taskSize);
        break;
    }

    default:
        raytracer = std::make_unique<RtCpu>(image, cam, scene, samples);
    }

    if (profile) {
        int counts = image.width * image.height * samples * 4;
        Profile Instance([&]() { raytracer->render(); },
                         raytracer->name(), counts);
        Instance.run();
        Instance.print();
    } else {
        raytracer->render();
    }

    std::cout << "Image saved to image.ppm" << std::endl;
    // output
    image.save();
}