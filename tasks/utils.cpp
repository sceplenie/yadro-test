#include <cmath>
#include <random>
#include <chrono>

#include "utils.h"

namespace utils {

double GetNormalizationFactor(int modulation_mode) {
    switch (modulation_mode) {
    case 1:  // QPSK
        return 1.0 / std::sqrt(2);
    case 2:  // 16-QAM
        return 1.0 / std::sqrt(10);
    case 3:  // 64-QAM
        return 1.0 / std::sqrt(42);
    default:
        throw std::invalid_argument("Unsupported modulation mode");
    }
}

std::vector<int> GenerateRandomBits(int size) {
    std::vector<int> result(size);
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count() ^ (uint64_t)std::random_device{}();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(0, 1);

    for (int i = 0; i < size; ++i)
        result[i] = dist(gen);

    return result;
}

}