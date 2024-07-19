#pragma once
#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

enum class ExecuteMode {
    CPU,
    SoA,
    SIMD_SSE,
    SIMD_AVX,
    SIMD_AVX_512,
    RawThread,
    OpenMP,
    Async,
};

enum class ThreadLoadType {
    Row,
    Column,
    Block,
};

enum class OpenMPMode {
    ParallelFor,
    ParallelTask,
    ParallelWorkStealing,
};

class Application {
  public:
    Application();
    ~Application(){};
    int run(int argc, char **argv);
    ThreadLoadType getThreadLoadType();
    ExecuteMode getExecuteMode();
    OpenMPMode getOpenMPMode();
    void print();
    std::string name() const { return executeMode; }
    std::string getLogLevel() const { return logLevel; }

  public:
    CLI::App app;
    std::string output = "image.ppm";
    int width = 512;
    int height = 512;
    int samples = 1;
    int threads = 8;
    bool profile = true;
    int taskSize = 1 * 1;
    std::string scene = "basic";

  private:
    std::string loadType = "Row";
    std::string openMPMode = "ParallelFor";
    std::string executeMode = "CPU";
    std::string logLevel = "off";
};