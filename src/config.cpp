#include "config.h"
Application::Application() {
    app.description("Parallel Path Tracing in C++ with SIMD|SoA|OpenMP|CUDA");
    app.name("PathTracer");

    app.add_option("-o,--output", output, "Output file name");
    app.add_option("--width", width, "Image width");
    app.add_option("--height", height, "Image height");
    app.add_option("-s,--samples", samples, "Samples per pixel");
    app.add_option("-t,--threads", threads, "Number of threads");
    app.add_option("-m,--mode", executeMode, "Mode: CPU, SoA, SIMD_SSE, SIMD_AVX, SIMD_AVX_512, RawThread, OpenMP, Async")->required();
    app.add_option("-p,--profile", profile, "Profile mode");
    app.add_option("-l,--load", loadType, "Thread load type: Row, Column, Task");
    app.add_option("--task", taskSize, "Task size");
    app.add_option("--scene", scene, "Scene name");
    app.allow_extras();
}

int Application::run(int argc, char **argv) {
    CLI11_PARSE(app, argc, argv);
    return 0;
}

void Application::print() {
    std::cout << "Configuration:" << std::endl;
    std::cout << "Image: " << width << "x" << height << std::endl;
    std::cout << "Samples: " << samples << std::endl;
    std::cout << "ThreadNum: " << threads << std::endl;
    std::cout << "TaskSize: " << taskSize << std::endl;
    std::cout << "SplitThreadLoadType: " <<  loadType << std::endl;
    std::cout << "OpenMPMode: " << openMPMode << std::endl;
}

ExecuteMode Application::getExecuteMode() {
    if (executeMode == "CPU") {
        return ExecuteMode::CPU;
    } else if (executeMode == "SoA") {
        return ExecuteMode::SoA;
    } else if (executeMode == "SIMD_SSE") {
        return ExecuteMode::SIMD_SSE;
    } else if (executeMode == "SIMD_AVX") {
        return ExecuteMode::SIMD_AVX;
    } else if (executeMode == "SIMD_AVX_512") {
        return ExecuteMode::SIMD_AVX_512;
    } else if (executeMode == "RawThread") {
        return ExecuteMode::RawThread;
    } else if (executeMode == "OpenMP") {
        return ExecuteMode::OpenMP;
    } else if (executeMode == "Async") {
        return ExecuteMode::Async;
    } else {
        return ExecuteMode::CPU;
    }
}

ThreadLoadType Application::getThreadLoadType() {
    if (loadType == "Row") {
        return ThreadLoadType::Row;
    } else if (loadType == "Column") {
        return ThreadLoadType::Column;
    } else if (loadType == "Block") {
        return ThreadLoadType::Block;
    } else {
        return ThreadLoadType::Row;
    }
}

OpenMPMode Application::getOpenMPMode() {
    if (openMPMode == "ParallelFor") {
        return OpenMPMode::ParallelFor;
    } else if (openMPMode == "ParallelTask") {
        return OpenMPMode::ParallelTask;
    } else if (openMPMode == "ParallelWorkStealing") {
        return OpenMPMode::ParallelWorkStealing;
    } else {
        return OpenMPMode::ParallelFor;
    }
}