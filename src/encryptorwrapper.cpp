#include <fstream>

#include <glog/logging.h>

#include "encryptor.h"
#include "utils.h"
#include "random.h"
#include "certificate.h"
#include "asym_cipher.h"
#include "log.h"

#ifdef __cpluscplus
extern "C" {
#endif

void init(const char* logpath) {
    initLog(logpath);
}

int encrypt(const char* inputfile, const char* outputfile, const char* certificate) {
    ssl::AesEncryptor encryptor;
    std::vector<uint8_t> iv;
    if (!ssl::generateRandom(16, iv)) {
        LOG(FATAL) << "Failed to generate iv: " << ssl::getOpensslErrStr();
    }

    std::vector<uint8_t> key;
    if (!ssl::generateRandom(16, key)) {
        LOG(FATAL) << "Failed to generate aes for blackbox: " << ssl::getOpensslErrStr();
    }

    if (!encryptor.initialize(key, iv)) {
        LOG(FATAL) << "Failed to initialize aes encryptor: " << ssl::getOpensslErrStr();
    }

    auto plaintext = Utils::read(inputfile);
    if (plaintext.empty()) {
        LOG(FATAL) << "Target file empty or not exists.";
    }
    std::vector<uint8_t> ciphertext;

    if (!encryptor.update(plaintext, ciphertext)) {
        LOG(ERROR) << "Failed to encrypt update msg:" << ssl::getOpensslErrStr();
        return -1;
    }

    if (!encryptor.finalize(ciphertext)) {
        LOG(ERROR) << "Failed to finalize aes encryptor:" << ssl::getOpensslErrStr();
        return -1;
    }

    const char* certi = "/etc/secflash/secflash.pem";
    if (certificate != nullptr) {
        certi = certificate;
    }
    ssl::Certificate cert(certi);
    std::vector<uint8_t> cipherkey;
    ssl::AsymCipher asym(cert.publicKey());

    if (!asym.init()) {
        LOG(ERROR) << "Failed to initialize asym encryptor: " << ssl::getOpensslErrStr();
        return -1;
    }
    
    if (!asym.encrypt(key, cipherkey)) {
        LOG(ERROR) << "Asym encryptor failed to encrypt: " << ssl::getOpensslErrStr();
        return -1;
    }    

    Utils::Tlvs tlvs;
    tlvs.append(0x5555, cipherkey);
    tlvs.append(0xaaaa, iv);
    tlvs.append(0x55aa, ciphertext);
    tlvs.writeToFile(outputfile);

    LOG(INFO) << "blackbox is encrypted.";
    return 0;
}

#ifdef __cpluscplus
}
#endif