#pragma once

#include <vector>
#include <cstdint>

#include "sslwrapper.h"

namespace ssl {

class AsymCipher {
public:
    AsymCipher(UniEvpPkey&& key) : key_(std::move(key)), ctx_(nullptr) {}

    bool init();
    bool encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext);

private:
    UniEvpPkey key_;
    UniEvpPkeyCtx ctx_;
};

}