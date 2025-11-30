#pragma once
#include <vector>
#include <cstdint>

std::vector<uint8_t> ecb_encrypt(const std::vector<uint8_t>& key, const std::vector<uint8_t>& plaintext);
std::vector<uint8_t> ecb_decrypt(const std::vector<uint8_t>& key, const std::vector<uint8_t>& ciphertext);