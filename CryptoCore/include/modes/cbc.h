#pragma once
#include <vector>
#include <cstdint>

namespace modes {
    std::vector<uint8_t> encrypt_cbc(const std::vector<uint8_t>& key, const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& iv);
    std::vector<uint8_t> decrypt_cbc(const std::vector<uint8_t>& key, const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv);
}