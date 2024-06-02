#pragma once
#include "rt_helper.h"
#include <vector>

auto genRays(int w, int h, int s) -> std::vector<Ray> {
    std::vector<Ray> rays;
    Ray camera(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm());
    Vec cx = Vec(w * .5135 / h);
    Vec cy = (cx.cross(camera.d)).norm() * .5135;

    printVec("cx", cx);
    printVec("cy", cy);

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            for (int sy = 0; sy < 2; sy++) {
                for (int sx = 0; sx < 2; sx++) {
                    Vec r;
                    for (int k = 0; k < s; k++) {
                        FF r1 = 2 * randomFF();
                        FF dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        FF r2 = 2 * randomFF();
                        FF dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                        Vec d = cx * (((sx + .5 + dx) / 2 + i) / w - .5) +
                                cy * (((sy + .5 + dy) / 2 + j) / h - .5) +
                                camera.d;

                        rays.push_back(Ray(camera.o + d * 140, d.norm()));
                    }
                }
            }
        }
    }
    return rays;
}

// select the origin of the ray as the color
auto stdColor(const std::vector<Ray> &rays) -> std::vector<Vec> {
    std::vector<Vec> colors;
    for (const auto &ray : rays) {
        Vec color = radiance(ray, 0);
        colors.push_back(color);
    }
    return colors;
}

auto showDiff(const std::vector<Vec> &outputColor,
              const std::vector<Vec> &stdColorsData,
              bool printError = false) -> int {
    int elementNums = outputColor.size();
    // compare
    bool flag = true;
    for (int i = 0; i < elementNums; i++) {
        float diff = fabs(outputColor[i].x - stdColorsData[i].x) +
                     fabs(outputColor[i].y - stdColorsData[i].y) +
                     fabs(outputColor[i].z - stdColorsData[i].z);
        if (diff > 0.001) {
            if (printError) {
                printf("Error: %d output: (%.3f,%.3f,%.3f)  "
                       "except:(%.3f,%.3f,%.3f) \n",
                       i, outputColor[i].x, outputColor[i].y, outputColor[i].z,
                       stdColorsData[i].x, outputColor[i].y, outputColor[i].z);
            }
            flag = false;
        }
    }
    return flag;
}