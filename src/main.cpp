#include "camera.h"
#include "debug_helper.h"
#include "image.h"
#include "parallel/simd/rt_simd.h"
#include "parallel/simd/simd_helper.h"
#include "parallel/soa/rt_soa.h"
#include "parallel/thread/rt_openmp.h"
#include "parallel/thread/rt_thead.h"
#include "profile.h"
#include "raytracer.h"
#include "rt_cpu.h"
#include "scene.h"
#include <iostream>

int main(int argc, char *argv[]) {

    // configuration
    Application app;
    app.run(argc, argv);

    DebugHelper::init(app.name(), app.getLogLevel());

    Image image(app.output, app.width, app.height);
    Camera cam = Camera(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm(), image);
    Scene scene = CornelBoxWithPlantySphere(64);
    int samples = app.samples; // 2 * 2 * samples per pixel
    bool profile = app.profile;
    int threadNum = app.threads;
    ThreadLoadType splitThreadLoadType = app.getThreadLoadType();
    OpenMPMode openmpMode = app.getOpenMPMode();
    int taskSize = app.taskSize;

    // print configuration
    app.print();

    // render
    std::unique_ptr<RayTracer> raytracer;
    switch (app.getExecuteMode()) {
    case ExecuteMode::CPU: {
        std::cout << "Mode: CPU" << std::endl;
        raytracer = std::make_unique<RtCpu>(image, cam, scene, samples);
        break;
    }
    case ExecuteMode::SoA: {
        std::cout << "Mode: SoA" << std::endl;
        raytracer = std::make_unique<RayTracingSoA>(image, cam, scene, samples);
        break;
    }
    case ExecuteMode::SIMD_SSE: {
        std::cout << "Mode: SIMD_SSE" << std::endl;
        raytracer = std::make_unique<RayTracingSIMD>(image, cam, scene, samples);
        break;
    }
    case ExecuteMode::RawThread: {
        std::cout << "Mode: RawThread" << std::endl;
        raytracer = std::make_unique<RtThread>(image, cam, scene, samples, threadNum, taskSize, splitThreadLoadType);
        break;
    }

    case ExecuteMode::OpenMP: {
        std::cout << "Mode: OpenMP" << std::endl;
        raytracer = std::make_unique<RtOpenMP>(image, cam, scene, samples, openmpMode, taskSize);
        break;
    }

    default:
        std::cout << "Unknown Mode, Select Default Mode CPU" << std::endl;
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
    spdlog::shutdown();
}