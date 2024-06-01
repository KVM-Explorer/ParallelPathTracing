#include "data_gen.h"
// #include "data_utils.h"

const int32_t WIDTH = 512;
const int32_t HEIGHT = 512;
const int32_t SAMPLES = 1; // SAMPLES * 4 = total samples

#ifndef __CCE_KT_TEST__
#include <acl/acl.h>
extern void render(uint32_t coreDim, void *l2ctrl, void *stream, uint8_t *rays, uint8_t *colors);
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

    std::vector<Ray>
        raysData = genRays(elementNums);
    std::vector<Vec> colorsData = stdColor(raysData);

    uint8_t *rays = (uint8_t *)AscendC::GmAlloc(inputByteSize);
    uint8_t *colors = (uint8_t *)AscendC::GmAlloc(inputByteSize);

    AscendC::SetKernelMode(KernelMode::AIV_MODE);

    ICPU_RUN_KF(render, blockDim, rays, colors);

    AscendC::GmFree((void *)rays);
    AscendC::GmFree((void *)colors);
#else

#endif
}