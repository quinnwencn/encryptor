#include "utils.h"

#include <iostream>
#include <iomanip>
#include <fstream>

#include <unistd.h>

namespace Utils {

int read(const std::string& file, std::string& output) {
    if (::access(file.c_str(), F_OK) != 0) {
        return -1;
    }

    std::ifstream ifs {file};
    if (!ifs.good()) {
        return -1;
    }

    std::string content {std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
    output.swap(content);
    return 0;
}

std::vector<uint8_t> read(const std::string& file) {
    std::ifstream ifs {file};
    if (!ifs.good()) {
        std::cerr << "open file error" << std::endl;
        return {};
    }
 
    ifs.seekg(0, std::ios::end);
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer;
    buffer.resize(size);

    if (!ifs.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Error reading file." << std::endl;

        return {};
    }

    return buffer;
}

void Tlvs::append(uint16_t type, const std::vector<uint8_t>& value) {
    tlvs_.push_back(Tlv(type, value));
}

bool Tlvs::writeToFile(const std::string& filename) {
    std::ofstream ofs {filename};
    if (!ofs.good()) {
        std::cout << "Failed to open write file: " << filename << std::endl;
        return false;
    }

    for (auto& tlv: tlvs_) {
        ofs.write(reinterpret_cast<char*>(&tlv.type_), sizeof(tlv.type_));
        uint32_t length = static_cast<uint32_t>(tlv.value_.size());
        ofs.write(reinterpret_cast<const char*>(&length), sizeof(length));
        for (auto c: tlv.value_) {
            ofs << c;
        }
    }

    ofs.flush();
    return true;
}

}