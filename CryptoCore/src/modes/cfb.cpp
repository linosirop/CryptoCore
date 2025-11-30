#include "modes/cfb.h"

#include <stdexcept>
#include <algorithm>


#include "aes_primitive.h"

namespace modes {
    std::vector<uint8_t> encrypt_cfb(const std::vector<uint8_t>& key, const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& iv) {
        std::vector<uint8_t> ciphertext;
        ciphertext.reserve(plaintext.size());
        std::vector<uint8_t> reg = iv;

        for (size_t i = 0; i < plaintext.size(); i += 16) {
            reg = aes_encrypt_block(key, reg);
            size_t block_size = std::min<size_t>(16, plaintext.size() - i);
            for (size_t j = 0; j < block_size; ++j) {
                uint8_t c = plaintext[i + j] ^ reg[j];
                ciphertext.push_back(c);
                reg[j] = c;
            }
        }
        return ciphertext;
    }

    std::vector<uint8_t> decrypt_cfb(const std::vector<uint8_t>& key, const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv) {
        std::vector<uint8_t> plaintext;
        plaintext.reserve(ciphertext.size());
        std::vector<uint8_t> reg = iv;

        for (size_t i = 0; i < ciphertext.size(); i += 16) {
            reg = aes_encrypt_block(key, reg);
            size_t block_size = std::min<size_t>(16, ciphertext.size() - i);
            for (size_t j = 0; j < block_size; ++j) {
                uint8_t p = ciphertext[i + j] ^ reg[j];
                plaintext.push_back(p);
                reg[j] = ciphertext[i + j];
            }
        }
        return plaintext;
    }
}