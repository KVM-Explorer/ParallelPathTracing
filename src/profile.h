#pragma once
#include <functional>
#include <string>
#include <chrono>
#include <iostream>
#include <format>

class Profile {

    std::function<void()> func;
    std::string name;
    int counts;
    double time;

  public:
    // Profile constructor with function, name and compute pixel counts
    Profile(std::function<void()> func, std::string name, int counts) : func(func), name(name), counts(counts) {
    }

    void run() {
        time = 0;
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        time += std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    }

    void print() {
        std::cout << std::format("Profile: {} time: {:.2f}s, single ray {:.2f}ns \n", name, time/1e9, time / counts);
    }

};