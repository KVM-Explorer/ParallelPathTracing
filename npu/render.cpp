#include "rt_helper.h"
using namespace AscendC;

constexpr int32_t TOTAL_NUM = WIDTH * HEIGHT * SAMPLES * 4;
constexpr int32_t USE_CORE_NUM = 8;
constexpr int32_t TILING_NUM = 8;
constexpr int32_t BUFFER_NUM = 4;

class KernelRender {

  public:
    __aicore__ inline KernelRender() {}
    __aicore__ inline void Init(int w, int h, int s, GM_ADDR input,
                                 GM_ADDR output) {

        width = w;
        height = h;
        samples = s;

        constexpr int32_t block_length = TOTAL_NUM / USE_CORE_NUM;

        int32_t block_offset = block_length * GetBlockIdx();
        // 数组式访问，而非指针式访问
        inputRays.SetGlobalBuffer((__gm__ Ray *)input + block_offset,
                                  block_length);

        resultColor.SetGlobalBuffer((__gm__ Vec *)output + block_offset,
                                    block_length);

        constexpr int32_t tiling_length = TOTAL_NUM / USE_CORE_NUM / TILING_NUM;
        pipe.InitBuffer(inQueueX, BUFFER_NUM, tiling_length * sizeof(Ray));
        pipe.InitBuffer(outQueueY, BUFFER_NUM, tiling_length * sizeof(Vec));
    }

    __aicore__ inline void Process() {

        int loop_count = TILING_NUM * BUFFER_NUM;
        // constexpr int32_t ONCE_NUM =
        //     TOTAL_NUM / USE_CORE_NUM / TILING_NUM / BUFFER_NUM;
        for (int i = 0; i < loop_count; i++) {
            CopyIn(i);
            Compute(i);
            CopyOut(i);
        }
    }

    __aicore__ inline void Release() {
        // free local tensor
    }

  private:
    // system mem -> device mem
    __aicore__ inline void CopyIn(int32_t progress) {
        constexpr int32_t tiling_length =
            TOTAL_NUM / USE_CORE_NUM / TILING_NUM / BUFFER_NUM;

        // generate rays
        LocalTensor<Ray> cur_ray = inQueueX.AllocTensor<Ray>();
        // DataCopy(cur_ray, inputRays[progress * TILE_LENGTH], TILE_LENGTH);
        DataCopy(cur_ray, inputRays[progress * tiling_length], tiling_length);
        inQueueX.EnQue(cur_ray);
    }

    // read device mem & compute & output to device queue & all samples
    __aicore__ inline void Compute(int32_t progress) {

        LocalTensor<Vec> ret = outQueueY.AllocTensor<Vec>();
        LocalTensor<Ray> ray = inQueueX.DeQue<Ray>();

        constexpr int32_t data_num =
            TOTAL_NUM / USE_CORE_NUM / TILING_NUM / BUFFER_NUM;
        // const int32_t data_offset = data_num * progress;

        Vec color(0, 0, 0);
        for (int i = 0; i < data_num; i++) {
            auto cur_ray = ray.GetValue(i);
            // color = radiance(cur_ray, 0);
            color = radiance(cur_ray, 0);
            // #ifdef __CCE_KT_TEST__
            //             printf("color: (%.5f, %.5f, %.5f)\n", color.x,
            //             color.y, color.z);
            // #endif
            ret.SetValue(i, color);
        }

        inQueueX.FreeTensor(ray);
        outQueueY.EnQue<Vec>(ret);
    }

    // write device queue to system mem
    __aicore__ inline void CopyOut(int32_t progress) {
        // deque output tensor from VECOUT queue
        LocalTensor<Vec> color = outQueueY.DeQue<Vec>();
        // copy progress_th tile from local tensor to global tensor
        constexpr int32_t tiling_length =
            TOTAL_NUM / USE_CORE_NUM / TILING_NUM / BUFFER_NUM;

        DataCopy(resultColor[progress * tiling_length], color, tiling_length);
        // free output tensor for reuse
        outQueueY.FreeTensor(color);
    }

  private:
    int width;
    int height;
    int samples;
    // GlobalTensor<Sphere> spheres;
    // LocalTensor<Sphere> localSpheres;
    GlobalTensor<Vec> resultColor;
    GlobalTensor<Ray> inputRays;

    Vec cx, cy;
    // 输入队列
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueX;
    // 输出队列
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueY;

    TPipe pipe;
};

extern "C" __global__ __aicore__ void render(GM_ADDR input, GM_ADDR output) {
    KernelRender op;

    op.Init(WIDTH, HEIGHT, SAMPLES, input, output);
    op.Process();
    op.Release();
}

#ifndef __CCE_KT_TEST__
// call of kernel function
void render_do(uint32_t blockDim, void *l2ctrl, void *stream, uint8_t *input,
               uint8_t *output) {
    add_custom<<<blockDim, l2ctrl, stream>>>(WIDTH, HEIGHT, SAMPLES, input,
                                             output);
}
#endif
