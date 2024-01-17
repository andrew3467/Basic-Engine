//
// Created by apgra on 1/15/2024.
//

#ifndef BASIC_ENGINE_UTILS_H
#define BASIC_ENGINE_UTILS_H

namespace BE {
    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };
}

#endif //BASIC_ENGINE_UTILS_H
