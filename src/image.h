#pragma once
#include "utils/types.h"
#include "rt_math.h"
#include <format>
#include <fstream>
#include <iostream>
#include <vector>

class Image {
  public:
    const int width;
    const int height;
    std::ofstream file;

    Image(const std::string name, int width, int height)
        : width(width),
          height(height),
          data(width * height, 0),
          file(name, std::ios::out | std::ios::binary) {
        file << "P3\n"
             << width << " " << height << "\n255\n";
    }

    ~Image() {
        file.close();
    }

    void write(int x, int y, const Vec &color) {
        if (x > width || y > height || x < 0 || y < 0) {
            std::cout << std::format("Error: Out of bounds ({}, {})\n", x, y);
            return;
        }
        int index = (y * width + x);
        data[index] = data[index] + color;
    }



    void save() {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) {
                int index = (i * width + j);
                file << std::format("{} {} {}\n",
                                    convert(data[index].x),
                                    convert(data[index].y),
                                    convert(data[index].z));
            }
    }

  private:
    std::vector<Vec> data;

    uint8_t convert(Float value) {
        return int(pow(clamp(value), 1 / 2.2) * 255 + .5);
    }

    
};