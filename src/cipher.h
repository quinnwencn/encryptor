#ifndef CIPHER_H
#define CIPHER_H

#include <vector>
#include <cstdint>

#include "sslwrapper.h"

namespace ssl {

class AesCipher {
public:
    AesCipher() : ctx_(::EVP_CIPHER_CTX_new()), len_(0) {}
    ~AesCipher() = default;

    virtual bool initialize(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) = 0;
    virtual bool update(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext) = 0;
    virtual bool finalize(std::vector<uint8_t>& ciphertext) = 0;

protected:
    UniCipher ctx_;
    int len_;
};

}

#endif // CIPHER_H
