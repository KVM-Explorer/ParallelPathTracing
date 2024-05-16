#include <iostream>
#include <rt_cpu.h>
#include <utils/image.h>
#include "camera.h"
#include "raytracer.h"

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
    Mode mode = Mode::CPU;
    Camera cam = Camera(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm(),image);
    Scene scene = CornellBox();
    int samples = 1;

    // render
    std::unique_ptr<RayTracer> raytracer;
    switch (mode) {
    case Mode::CPU:
        raytracer = std::make_unique<RtCpu>();
        break;
    default:
        raytracer = std::make_unique<RtCpu>();
    }
     raytracer->render(image, cam, scene,samples);


    std::cout<< "Image saved to image.ppm" << std::endl;
    // output
    image.save();
    
}