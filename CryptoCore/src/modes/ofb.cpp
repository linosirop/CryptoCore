#include "modes/ofb.h"

#include <stdexcept>
#include <algorithm>
#include "padding.h"

#include "aes_primitive.h"

namespace modes {
    std::vector<uint8_t> encrypt_ofb(const std::vector<uint8_t>& key, const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& iv) {
        std::vector<uint8_t> ciphertext;
        ciphertext.reserve(plaintext.size());
        std::vector<uint8_t> reg = iv;
        for (size_t i = 0; i < plaintext.size(); i += 16) {
            reg = aes_encrypt_block(key, reg);
            size_t block_size = std::min<size_t>(16, plaintext.size() - i);
            for (size_t j = 0; j < block_size; ++j) {
                ciphertext.push_back(plaintext[i + j] ^ reg[j]);
            }
        }
        return ciphertext;
    }

    std::vector<uint8_t> decrypt_ofb(const std::vector<uint8_t>& key, const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv) {
        return encrypt_ofb(key, ciphertext, iv); // OFB: encrypt = decrypt
    }
}