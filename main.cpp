#include <fstream>
#include <iostream>

#include <getopt.h>
#include <glog/logging.h>

#include "encryptor.h"
#include "utils.h"
#include "random.h"
#include "certificate.h"
#include "asym_cipher.h"
#include "log.h"

const std::string VERSION {"1.0.0"};

void print_help() {
    std::cout << "Usage:\n";
    std::cout << "      encryptor options.\n";
    std::cout << "Options:\n";
    std::cout << "    -b blackbox           path of blackbox\n";
    std::cout << "    -c certificate        path of certificate\n";
    std::cout << "    -o output             path of output encrypted file\n";
    std::cout << "    -v version            encryptor version\n";
    std::cout << "    -h help               usage help\n";
}

void print_version() {
    std::cout << "encryptor " << VERSION << std::endl;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    struct option options[] = {
        {"blackbox", required_argument, NULL, 'b'},
        {"certificate", required_argument, NULL, 'c'},
        {"output", required_argument, NULL, 'o'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
    };

    int opt = -1;
    int index = -1;
    char* blackbox = nullptr;
    char* certificate = nullptr;
    char* output = nullptr;
    while ((opt = getopt_long(argc, argv, "b:c:o:v:h", options, &index)) != -1) {
        switch (opt) {
            case 'b':
                blackbox = optarg;
                break;
            case 'c':
                certificate = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 'v':
                print_version();
                break;
            case 'h':
                print_help();
                break;
            default:
                std::cout << "Usage %s -b <path> -c <path> -o <path>\n";
                print_help();
                return 1;
        }
    }

    initLog("test.log");
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
        std::cout << ssl::getOpensslErrStr() << std::endl;
        LOG(FATAL) << "Failed to initialize aes encryptor: " << ssl::getOpensslErrStr();
    }

    auto plaintext = Utils::read(blackbox);
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

    ssl::Certificate cert(certificate);
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

    std::cout << "cipherkey's size:" << std::hex << cipherkey.size() << std::endl;

    Utils::Tlvs tlvs;
    tlvs.append(0x5555, cipherkey);
    tlvs.append(0xaaaa, iv);
    tlvs.append(0x55aa, ciphertext);
    tlvs.writeToFile(output);

    LOG(INFO) << "blackbox is encrypted.";

    return 0;
}