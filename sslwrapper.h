#pragma once
#include <memory>
#include <string>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

namespace ssl {

#define DELETER(Type, FreeFunc)             \
    void operator()(::Type* ptr) const {    \
        ::FreeFunc(ptr);                    \
    }

struct SslFree {
    DELETER(X509, X509_free)
    DELETER(EVP_PKEY, EVP_PKEY_free)
    DELETER(EVP_PKEY_CTX, EVP_PKEY_CTX_free)
    DELETER(X509_REQ, X509_REQ_free)
    DELETER(EVP_MD_CTX, EVP_MD_CTX_destroy) // 对应 OpenSSL 1.0.2j
    DELETER(BIGNUM, BN_free)
    DELETER(BIO, BIO_free_all)
    DELETER(X509_NAME, X509_NAME_free)
    DELETER(EVP_CIPHER_CTX, EVP_CIPHER_CTX_free)
};

using UniX509 = std::unique_ptr<::X509, SslFree>;
using UniBio = std::unique_ptr<::BIO, SslFree>;
using UniEvpPkey = std::unique_ptr<::EVP_PKEY, SslFree>;
using UniX509Name = std::unique_ptr<::X509_NAME, SslFree>;
using UniBn = std::unique_ptr<::BIGNUM, SslFree>;
using UniEvpPkeyCtx = std::unique_ptr<::EVP_PKEY_CTX, SslFree>;
using UniX509Req = std::unique_ptr<::X509_REQ, SslFree>;
using UniEvpMdCtx = std::unique_ptr<::EVP_MD_CTX, SslFree>;
using UniCipher = std::unique_ptr<::EVP_CIPHER_CTX, SslFree>;

inline std::string getOpensslErrStr() {
    char buf[255];
    auto errorCode = ::ERR_get_error();
    ::ERR_error_string_n(errorCode, buf, sizeof(buf));
    return std::string(buf);
}

} // namespace ssl

