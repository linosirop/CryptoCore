#define OPENSSL_SUPPRESS_DEPRECATED
#include "aes_primitive.h"
#include <openssl/aes.h>
#include <stdexcept>

std::vector<uint8_t> aes_encrypt_block(const std::vector<uint8_t>& key, const std::vector<uint8_t>& block) {
    if (key.size() != 16 || block.size() != 16)
        throw std::runtime_error("Block must be exactly 16 bytes");

    AES_KEY aes_key;
    if (AES_set_encrypt_key(key.data(), 128, &aes_key) < 0)
        throw std::runtime_error("AES_set_encrypt_key failed");

    std::vector<uint8_t> out(16);
    AES_encrypt(block.data(), out.data(), &aes_key);
    return out;
}

std::vector<uint8_t> aes_decrypt_block(const std::vector<uint8_t>& key, const std::vector<uint8_t>& block) {
    if (key.size() != 16 || block.size() != 16)
        throw std::runtime_error("Block must be exactly 16 bytes");

    AES_KEY aes_key;
    if (AES_set_decrypt_key(key.data(), 128, &aes_key) < 0)
        throw std::runtime_error("AES_set_decrypt_key failed");

    std::vector<uint8_t> out(16);
    AES_decrypt(block.data(), out.data(), &aes_key);
    return out;
}