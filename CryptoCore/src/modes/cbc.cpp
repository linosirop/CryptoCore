#include "modes/cbc.h"
#include <openssl/evp.h>
#include <stdexcept>

#include "modes/cbc.h"
#include "aes_primitive.h"
#include "padding.h"

namespace modes {
    std::vector<uint8_t> encrypt_cbc(const std::vector<uint8_t>& key, const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& iv) {
        auto padded = pkcs7_pad(plaintext, 16);
        std::vector<uint8_t> ciphertext;
        ciphertext.reserve(padded.size());
        std::vector<uint8_t> prev = iv;

        for (size_t i = 0; i < padded.size(); i += 16) {
            std::vector<uint8_t> block(padded.begin() + i, padded.begin() + i + 16);
            for (size_t j = 0; j < 16; ++j) block[j] ^= prev[j];
            prev = aes_encrypt_block(key, block);
            ciphertext.insert(ciphertext.end(), prev.begin(), prev.end());
        }
        return ciphertext;
    }

    std::vector<uint8_t> decrypt_cbc(const std::vector<uint8_t>& key, const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv) {
        if (ciphertext.size() % 16 != 0) throw std::runtime_error("Invalid ciphertext size");
        std::vector<uint8_t> plaintext;
        plaintext.reserve(ciphertext.size());
        std::vector<uint8_t> prev = iv;

        for (size_t i = 0; i < ciphertext.size(); i += 16) {
            std::vector<uint8_t> block(ciphertext.begin() + i, ciphertext.begin() + i + 16);
            std::vector<uint8_t> decrypted = aes_decrypt_block(key, block);
            for (size_t j = 0; j < 16; ++j) decrypted[j] ^= prev[j];
            plaintext.insert(plaintext.end(), decrypted.begin(), decrypted.end());
            prev = block;
        }
        return pkcs7_unpad(plaintext);
    }
}