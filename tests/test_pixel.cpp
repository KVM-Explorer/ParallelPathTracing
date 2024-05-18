#include "../lib/catch2/catch_amalgamated.hpp"
#include "../src/rt_cpu.h"

// #define RANDOM_FROZEN 1

TEST_CASE("Pixel color test","[pixel_color_test]") {
    int x = 5;
    int y = 0;
    int subx = 0;
    int suby = 1;
    int s = 0;
    Vec d = Vec(-0.311788, -0.274857, -0.909529);
    unsigned short Xi[3] = {};

    auto scene = CornellBox();
    
    // Vec std = RtCpu::tracing(Ray(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm()), 0,scene);

    // printf("std: (%lf,%lf,%lf)\n",std.x,std.y,std.z);   
}