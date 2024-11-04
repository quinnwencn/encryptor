#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <vector>
#include <cstdint>

namespace Utils {

std::vector<uint8_t> read(const std::string& file);
int read(const std::string& path, std::string& output);

class Tlvs {
public:
    Tlvs() = default;
    void append(uint16_t type, const std::vector<uint8_t>& value);
    bool writeToFile(const std::string& filename);

private:
    struct Tlv {
    Tlv(uint16_t type, const std::vector<uint8_t>& value): type_(type), value_(value) {}
    
    uint16_t type_;
    const std::vector<uint8_t>& value_;
    };

    std::vector<Tlv> tlvs_;
};

}

#endif
