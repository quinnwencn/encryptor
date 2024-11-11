#pragma once

#include <vector>
#include <cstdint>

#include <openssl/rand.h>

namespace ssl {

inline bool generateRandom(uint16_t size, std::vector<uint8_t>& output) {
    if (output.size() == 0) {
        output.resize(size);
    }

    if (::RAND_bytes(output.data(), size) != 1) {
        return false;
    }

    return true;
}

}