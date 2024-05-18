#pragma once
#include <limits> // Include the <limits> header
#include <random>
using Float = float;

const Float PI = 3.14159265358979323846;
const Float INF = std::numeric_limits<Float>::infinity();

inline Float clamp(Float value) {
    return value < 0 ? 0 : value > 1 ? 1
                                     : value;

}

inline Float random() {
    #if RANDOM_FROZEN
        static Float random = 0.8;
        return random;
    #endif
    
    static std::uniform_real_distribution<Float> distribution(0.0, 1.0);
    static std::mt19937_64 generator;
    return distribution(generator);
}