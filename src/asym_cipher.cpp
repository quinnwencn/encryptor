#include "asym_cipher.h"

#include <glog/logging.h>

namespace ssl {

bool AsymCipher::init() {
    UniEvpPkeyCtx ctx {(::EVP_PKEY_CTX_new(key_.get(), nullptr)) };
    if (ctx == nullptr) {
        LOG(ERROR) << "Falied to allocate Evp key ctx" << getOpensslErrStr();
        return false;
    }

    ctx_ = std::move(ctx);

    if (::EVP_PKEY_encrypt_init(ctx_.get()) <= 0) {
        LOG(ERROR) << "Failed to init EVP PKEY encrypt" << getOpensslErrStr();
        return false;
    }

    return true;
}

bool AsymCipher::encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext) {
    size_t len = 0;
    if (::EVP_PKEY_encrypt(ctx_.get(), nullptr, &len, plaintext.data(), plaintext.size()) != 1) {
        LOG(ERROR) << "Asym encryptor failed to get ciphertext's size:"  << getOpensslErrStr();
        return false;
    }

    ciphertext.resize(len, 0);
    if (::EVP_PKEY_encrypt(ctx_.get(), ciphertext.data(), &len, plaintext.data(), plaintext.size()) != 1) {
        LOG(ERROR) << "Asym encryptor failed to encrypt: " << getOpensslErrStr();
        return false;
    }

    ciphertext.resize(len);
    return true;
}

}