#pragma once

#include <cstdint>
#include <random>
#include <algorithm>

namespace utils {

constexpr uint32_t FourCC(const char p[5]) {
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

template<typename T=uint8_t>
std::vector<T> GenerateRandomVec(size_t size) {
    static std::uniform_int_distribution<T> distribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    static std::default_random_engine generator;

    std::vector<T> data(size);
    std::generate(data.begin(), data.end(), []() { return distribution(generator); });
    return data;
}

}