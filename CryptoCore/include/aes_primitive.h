#pragma once
#include <vector>
#include <cstdint>

std::vector<uint8_t> aes_encrypt_block(const std::vector<uint8_t>& key, const std::vector<uint8_t>& block);
std::vector<uint8_t> aes_decrypt_block(const std::vector<uint8_t>& key, const std::vector<uint8_t>& block);