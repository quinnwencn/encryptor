#include "encryptor.h"

#include <glog/logging.h>

namespace ssl {

bool AesEncryptor::initialize(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    if (::EVP_EncryptInit_ex(ctx_.get(), ::EVP_aes_128_cbc(), nullptr, key.data(), iv.data()) != 1) {
        LOG(ERROR) << "Failed to init aes encryptor" << getOpensslErrStr();
        return false;
    }

    return true;
}

bool AesEncryptor::update(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext) {
    if (ciphertext.size() == 0) {
        ciphertext.resize(plaintext.size());
    }

    int len = 0;
    if (::EVP_EncryptUpdate(ctx_.get(), ciphertext.data() + len_, &len, plaintext.data(), plaintext.size()) != 1) {
        LOG(ERROR) << "Aes encryptr failed to update: " << getOpensslErrStr();
        return false;
    }

    len_ += len;
    return true;
}

bool AesEncryptor::finalize(std::vector<uint8_t>& ciphertext) {
    int len = 0;
    ciphertext.resize(len_ + EVP_CIPHER_block_size(EVP_aes_128_cbc()));
    if (::EVP_EncryptFinal_ex(ctx_.get(), ciphertext.data() + len_, &len) != 1) {
        LOG(ERROR) << "Aes encryptor failed to finalize: " << getOpensslErrStr();
        return false;
    }

    ciphertext.resize(len_ + len);
    return true;
}

}
