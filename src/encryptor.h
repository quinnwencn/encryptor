#ifndef ENCRYPTOR
#define ENCRYPTOR

#include "cipher.h"

namespace ssl {

class AesEncryptor : public AesCipher {
public:
    AesEncryptor() = default;
    ~AesEncryptor() = default;

    bool initialize(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) final;
    bool update(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext) final;
    bool finalize(std::vector<uint8_t>& ciphertext) final;
};

}

#endif // ENCRYPTOR
