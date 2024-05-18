#pragma once

#include <format>
#include <iostream>


inline void OutputStatus(std::string name, int y, int h, int sapmles, int w) {
    std::cout << std::format("\rRendering {0} ({1} spp) {2:5.2f}%", name, sapmles * 4, 100. * y / (h - 1));
}
