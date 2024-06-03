
#include "data_utils.h"
#include "data_gen.h"
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

    std::vector<Sphere> spheresData = cornelbox();


    size_t inputByteSize = elementNums * sizeof(Ray);
    size_t outputByteSize = elementNums * sizeof(Vec);
    size_t sphereByteSize = sizeof(Sphere) * spheresData.size();




#ifdef __CCE_KT_TEST__

    std::vector<Ray> raysData = genRays(WIDTH, HEIGHT, SAMPLES);

    std::vector<Vec> stdColorsData = stdColor(raysData);
    std::vector<Vec> outputColor(elementNums);


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

#else

    aclrtContext context;
    int32_t deviceId = 0;
    CHECK_ACL(aclrtSetDevice(deviceId));
    CHECK_ACL(aclrtCreateContext(&context, deviceId));
    aclrtStream stream = nullptr;
    CHECK_ACL(aclrtCreateStream(&stream));

    std::vector<Ray> raysData = genRays(WIDTH, HEIGHT, SAMPLES);
    std::vector<Vec> outputColor(elementNums);

    // uint8_t *xHost, *yHost, *zHost;
    uint8_t *xDevice, *yDevice, *zDevice;

     uint8_t *rays = (uint8_t *)AscendC::GmAlloc(inputByteSize);
    uint8_t *colors = (uint8_t *)AscendC::GmAlloc(outputByteSize);
    uint8_t *spheres = (uint8_t *)AscendC::GmAlloc(sphereByteSize);

    // CHECK_ACL(aclrtMallocHost((void **)(&xHost), inputByteSize));
    // CHECK_ACL(aclrtMallocHost((void **)(&yHost), inputByteSize));
    // CHECK_ACL(aclrtMallocHost((void **)(&zHost), outputByteSize));
    CHECK_ACL(aclrtMalloc((void **)&xDevice, inputByteSize,
                          ACL_MEM_MALLOC_HUGE_FIRST));
    // CHECK_ACL(aclrtMalloc((void **)&yDevice, inputByteSize,
    //                       ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_ACL(aclrtMalloc((void **)&zDevice, outputByteSize,
                          ACL_MEM_MALLOC_HUGE_FIRST));

    // ReadFile("./input/input_x.bin", inputByteSize, xHost, inputByteSize);
    // ReadFile("./input/input_y.bin", inputByteSize, yHost, inputByteSize);

    CHECK_ACL(aclrtMemcpy(xDevice, inputByteSize, raysData.data(), inputByteSize,
                          ACL_MEMCPY_HOST_TO_DEVICE));
    // CHECK_ACL(aclrtMemcpy(yDevice, inputByteSize, yHost, inputByteSize,
    //                       ACL_MEMCPY_HOST_TO_DEVICE));

    render(blockDim, nullptr, stream, xDevice, zDevice);
    CHECK_ACL(aclrtSynchronizeStream(stream));

    CHECK_ACL(aclrtMemcpy(outputColor.data(), outputByteSize, zDevice, outputByteSize,
                          ACL_MEMCPY_DEVICE_TO_HOST));
    // WriteFile("./output/output_z.bin", zHost, outputByteSize);

    CHECK_ACL(aclrtFree(xDevice));
    // CHECK_ACL(aclrtFree(yDevice));
    CHECK_ACL(aclrtFree(zDevice));
    // CHECK_ACL(aclrtFreeHost(xHost));
    // CHECK_ACL(aclrtFreeHost(yHost));
    // CHECK_ACL(aclrtFreeHost(zHost));

    CHECK_ACL(aclrtDestroyStream(stream));
    CHECK_ACL(aclrtDestroyContext(context));
    CHECK_ACL(aclrtResetDevice(deviceId));
    CHECK_ACL(aclFinalize());

#endif
}