#include "data_gen.h"
// #include "data_utils.h"
#include "scene.h"
#ifndef __CCE_KT_TEST__
#include <acl/acl.h>
extern void render(uint32_t coreDim, void *l2ctrl, void *stream, uint8_t *rays,
                   uint8_t *colors);
#else
#include "tikicpulib.h"
extern "C" __global__ __aicore__ void render(GM_ADDR rays, GM_ADDR colors);
#endif

int main() {

    uint32_t blockDim = 8;
    uint32_t elementNums = WIDTH * HEIGHT * 4 * SAMPLES;

    size_t inputByteSize = elementNums * sizeof(Ray);
    size_t outputByteSize = elementNums * sizeof(Vec);

#ifdef __CCE_KT_TEST__

    std::vector<Ray> raysData = genRays(WIDTH, HEIGHT, SAMPLES);

    std::vector<Vec> stdColorsData = stdColor(raysData);
    std::vector<Vec> outputColor(elementNums);
    std::vector<Sphere> spheresData = cornelbox();

    size_t sphereByteSize = sizeof(Sphere) * spheresData.size();

    uint8_t *rays = (uint8_t *)AscendC::GmAlloc(inputByteSize);
    uint8_t *colors = (uint8_t *)AscendC::GmAlloc(outputByteSize);
    uint8_t *spheres = (uint8_t *)AscendC::GmAlloc(sphereByteSize);

    // copy data_gen
    memcpy(rays, raysData.data(), inputByteSize);

    printf("raysData: %d\n", int(raysData.size()));
    printf("colorsData: %d\n", int(stdColorsData.size()));

    AscendC::SetKernelMode(KernelMode::AIV_MODE);

    ICPU_RUN_KF(render, blockDim, rays, colors);

    // copy result
    memcpy(outputColor.data(), colors, outputByteSize);

    AscendC::GmFree((void *)rays);
    AscendC::GmFree((void *)colors);

    // for (const auto &color : outputColor) {
    //     if (fabs(color.x) > 0.0001 || fabs(color.y) > 0.0001 ||
    //         fabs(color.z) > 0.0001)
    //         printVec("color", color);
    // }
    // int idx = stdColorsData.size() - 1;

    // for (const auto &color : stdColorsData) {
    //     if (fabs(color.x) > 0.0001 || fabs(color.y) > 0.0001 ||
    //         fabs(color.z) > 0.0001)
    //         printVec("color", color);
    // }
    // showDiff(outputColor, stdColorsData,true);

#else

#endif
}