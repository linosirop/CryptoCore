#include "padding.h"
#include <vector>
#include <cstdint>
#include <stdexcept>

std::vector<uint8_t> pkcs7_pad(const std::vector<uint8_t>& data, size_t block_size = 16) {
    size_t pad_len = block_size - (data.size() % block_size);  
    std::vector<uint8_t> padded = data;
    padded.insert(padded.end(), pad_len, static_cast<uint8_t>(pad_len));
    return padded;
}

std::vector<uint8_t> pkcs7_unpad(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("PKCS#7 unpad: empty data");
    }

    uint8_t pad_len = data.back();

    if (pad_len == 0 || pad_len > 16) {
        throw std::runtime_error("PKCS#7 unpad: invalid padding length");
    }

    if (data.size() < pad_len) {
        throw std::runtime_error("PKCS#7 unpad: data too short for padding");
    }

    
    for (size_t i = data.size() - pad_len; i < data.size(); ++i) {
        if (data[i] != pad_len) {
            throw std::runtime_error("PKCS#7 unpad: invalid padding bytes");
        }
    }

    return std::vector<uint8_t>(data.begin(), data.end() - pad_len);
}