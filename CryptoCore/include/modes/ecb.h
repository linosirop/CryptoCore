#pragma once
#include <vector>
#include <string>
#include <cstdint>

std::vector<uint8_t> hex_to_bytes(const std::string& hex);  // CLI-3 helper
std::vector<uint8_t> ecb_encrypt(const std::vector<uint8_t>& key, const std::vector<uint8_t>& plaintext);  // CRY-3
std::vector<uint8_t> ecb_decrypt(const std::vector<uint8_t>& key, const std::vector<uint8_t>& ciphertext);