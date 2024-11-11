#include <fstream>
#include <iostream>

#include <getopt.h>

#include "encryptorwrapper.h"

const std::string VERSION {"1.0.0"};

void print_help() {
    std::cout << "Usage:\n";
    std::cout << "      encryptor options.\n";
    std::cout << "Options:\n";
    std::cout << "    -b blackbox           path of blackbox\n";
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
        {"output", required_argument, NULL, 'o'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
    };

    int opt = -1;
    int index = -1;
    char* blackbox = nullptr;
    char* output = nullptr;
    while ((opt = getopt_long(argc, argv, "b:o:v:h", options, &index)) != -1) {
        switch (opt) {
            case 'b':
                blackbox = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 'v':
                print_version();
                return 0;
            case 'h':
                print_help();
                return 0;
            default:
                std::cout << "Usage %s -b <path> -o <path>\n";
                print_help();
                return 1;
        }
    }

    init("test.log");
    if (encrypt(blackbox, output) != 0) {
        std::cerr << "failed to encrypt " << blackbox << std::endl;
        return -1;
    }

    return 0;
}