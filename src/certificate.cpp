#include "certificate.h"

#include <glog/logging.h>

#include "utils.h"

namespace ssl {

UniX509 pemCertsToUniX509(const std::string& content) {
    if (content.empty()) {
        LOG(ERROR) << "content empty";
        return nullptr;
    }

    UniBio bio {::BIO_new(::BIO_s_mem())};
    if (!bio) {
        LOG(ERROR) << "Failed to allocate memory for bio: " << getOpensslErrStr();
        return nullptr;
    }

    ::BIO_puts(bio.get(), content.c_str());
    UniX509 cert {::PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr)};
    return cert;
}

Certificate::Certificate(const std::string& file) : good_(true) {
    std::string content;
    if (Utils::read(file, content) != 0) {
        LOG(ERROR) << "Failed to read: " << file;
        good_ = false;
        return;
    }

    cert_ = pemCertsToUniX509(content);
    if (cert_ == nullptr) {
        good_ = false;
    }
}

Certificate::Certificate(Certificate&& cert) {
    cert_.reset(cert.cert_.release());
    good_ = cert.good();
}

Certificate& Certificate::operator=(Certificate&& cert) {
    if (this == &cert) {
        return *this;
    }

    cert_.reset(cert.cert_.release());
    good_ = cert.good();
    return *this;
}

bool Certificate::isValid() const {
    const auto now = ::time(nullptr);
    const auto notBefore = X509_get_notBefore(get());
    const auto notAfter = X509_get_notAfter(get());
    return ASN1_TIME_to_time_t(notBefore) < now && ASN1_TIME_to_time_t(notAfter) > now;
}

bool Certificate::isSelfSigned() const {
    auto pubKey = publicKey();
    return ::X509_verify(get(), pubKey.get());
}

bool Certificate::isSignedBy(const Certificate& ca) {
    auto pubKey = ca.publicKey();
    return ::X509_verify(get(), pubKey.get());
}

ssl::UniEvpPkey Certificate::publicKey() const {
    return UniEvpPkey {::X509_get_pubkey(get())};
}

std::string Certificate::commonName() const {
    auto subjectName = ::X509_get_subject_name(get());
    if (subjectName == nullptr) {
        LOG(ERROR) << "Get subject failed: " << getOpensslErrStr();
        return {};
    }

    int index = ::X509_NAME_get_index_by_NID(subjectName, NID_commonName, -1);
    if (index == -1) {
        LOG(ERROR) << "Get commonName index failed: " << getOpensslErrStr();
        return {};
    }

    auto entry = ::X509_NAME_get_entry(subjectName, index);
    if (entry == nullptr) {
        LOG(ERROR) << "Get entry from index failed: " << getOpensslErrStr();
        return {};
    }

    auto data = ::X509_NAME_ENTRY_get_data(entry);
    if (data == nullptr) {
        LOG(ERROR) << "Get data from entry failed: " << getOpensslErrStr();
        return {};
    }

    uint8_t* utf8 {nullptr};
    int length = ::ASN1_STRING_to_UTF8(&utf8, data);
    if (length < 0) {
        ::OPENSSL_free(utf8);
        LOG(ERROR) << "Convert to utf8 failed: " << getOpensslErrStr();
        return {};
    }

    std::string commonName(reinterpret_cast<char*>(utf8), length);
    ::OPENSSL_free(utf8);
    return commonName;
}

time_t Certificate::ASN1_TIME_to_time_t(const ASN1_TIME* asn1Time) {
    struct tm tm;
    const char* str = (const char*)asn1Time->data;
    size_t i = 0;

    memset(&tm, 0, sizeof(tm));

    if (asn1Time->type == V_ASN1_UTCTIME) {
        tm.tm_year = (str[i++] - '0') * 10 + (str[i++] - '0');
        if (tm.tm_year < 70)
            tm.tm_year += 100;
    } else if (asn1Time->type == V_ASN1_GENERALIZEDTIME) {
        tm.tm_year = (str[i++] - '0') * 1000 + (str[i++] - '0') * 100 + (str[i++] - '0') * 10 + (str[i++] - '0');
        tm.tm_year -= 1900;
    }

    tm.tm_mon = (str[i++] - '0') * 10 + (str[i++] - '0') - 1;
    tm.tm_mday = (str[i++] - '0') * 10 + (str[i++] - '0');
    tm.tm_hour = (str[i++] - '0') * 10 + (str[i++] - '0');
    tm.tm_min = (str[i++] - '0') * 10 + (str[i++] - '0');
    tm.tm_sec = (str[i++] - '0') * 10 + (str[i++] - '0');

    return mktime(&tm);
}

} // namespace ssl
