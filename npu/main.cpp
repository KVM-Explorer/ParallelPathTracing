
#include "data_utils.h"
#include "common.h"

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


#ifdef __CCE_KT_TEST__

    size_t inputByteSize = elementNums * 4 * 8;
    size_t outputByteSize = elementNums * 4 * 4;

    uint8_t *rays = (uint8_t *)AscendC::GmAlloc(inputByteSize);
    uint8_t *colors = (uint8_t *)AscendC::GmAlloc(outputByteSize);

    // copy data_gen
    ReadFile("./input/rays.bin",inputByteSize,rays, inputByteSize);
    AscendC::SetKernelMode(KernelMode::AIV_MODE);

    ICPU_RUN_KF(render, blockDim, rays, colors);

    // copy result
    WriteFile("./output/color.bin", colors, outputByteSize);

    AscendC::GmFree((void *)rays);
    AscendC::GmFree((void *)colors);

#else

    size_t inputByteSize = elementNums * 4 * 8 ; // 
    size_t outputByteSize = elementNums * 4 * 4;

    aclrtContext context;
    int32_t deviceId = 0;
    CHECK_ACL(aclrtSetDevice(deviceId));
    CHECK_ACL(aclrtCreateContext(&context, deviceId));
    aclrtStream stream = nullptr;
    CHECK_ACL(aclrtCreateStream(&stream));

    uint8_t *rayHost,*colorHost;
    uint8_t *rayDevice,*colorDevice;

    CHECK_ACL(aclrtMallocHost((void**)(&rayHost),inputByteSize));
    CHECK_ACL(aclrtMallocHost((void**)(&colorHost),outputByteSize));
    

    // CHECK_ACL(aclrtMallocHost((void **)(&xHost), inputByteSize));
    // CHECK_ACL(aclrtMallocHost((void **)(&yHost), inputByteSize));
    // CHECK_ACL(aclrtMallocHost((void **)(&zHost), outputByteSize));

    // CHECK_ACL(aclrtMalloc((void **)&yDevice, inputByteSize,
    //                       ACL_MEM_MALLOC_HUGE_FIRST));


    CHECK_ACL(aclrtMalloc((void**)&rayDevice,inputByteSize,ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_ACL(aclrtMalloc((void**)&colorDevice,outputByteSize,ACL_MEM_MALLOC_HUGE_FIRST));

    // ReadFile("./input/input_x.bin", inputByteSize, xHost, inputByteSize);
    // ReadFile("./input/input_y.bin", inputByteSize, yHost, inputByteSize);



    CHECK_ACL(aclrtMemcpy(rayDevice,inputByteSize,rayHost,inputByteSize,ACL_MEMCPY_HOST_TO_DEVICE));

    // CHECK_ACL(aclrtMemcpy(yDevice, inputByteSize, yHost, inputByteSize,
    //                       ACL_MEMCPY_HOST_TO_DEVICE));

    render(blockDim, nullptr, stream, rayDevice, colorDevice);
    CHECK_ACL(aclrtSynchronizeStream(stream));

    // CHECK_ACL(aclrtMemcpy(outputColor.data(), outputByteSize, zDevice,
    //                       outputByteSize, ACL_MEMCPY_DEVICE_TO_HOST));
    CHECK_ACL(aclrtMemcpy(colorHost,outputByteSize,colorDevice,outputByteSize,ACL_MEMCPY_DEVICE_TO_HOST));

    WriteFile("./output/color.bin", colorHost, outputByteSize);

    CHECK_ACL(aclrtFree(rayDevice));
    // CHECK_ACL(aclrtFree(yDevice));
    CHECK_ACL(aclrtFree(colorDevice));
    // CHECK_ACL(aclrtFreeHost(xHost));
    // CHECK_ACL(aclrtFreeHost(yHost));
    // CHECK_ACL(aclrtFreeHost(zHost));

    CHECK_ACL(aclrtFreeHost(rayHost));
    CHECK_ACL(aclrtFreeHost(colorHost));

    CHECK_ACL(aclrtDestroyStream(stream));
    CHECK_ACL(aclrtDestroyContext(context));
    CHECK_ACL(aclrtResetDevice(deviceId));
    CHECK_ACL(aclFinalize());

#endif
}