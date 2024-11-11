#pragma once
#include <string>
#include <vector>

#include "sslwrapper.h"

namespace ssl {

class Certificate {
public:
    explicit Certificate(const std::string& path);

    Certificate(const Certificate&) = delete;
    Certificate& operator=(const Certificate&) = delete;

    Certificate(Certificate&&);
    Certificate& operator=(Certificate&&);

    bool good() const { return good_; }
    bool isValid() const;
    bool isSelfSigned() const;
    bool isSignedBy(const Certificate& ca);
    ssl::UniEvpPkey publicKey() const;
    X509* get() const { return cert_.get(); }
    std::string commonName() const;

    static time_t ASN1_TIME_to_time_t(const ASN1_TIME* asn1Time);

private:
    bool good_;
    ssl::UniX509 cert_;
};

} // namespace ssl
