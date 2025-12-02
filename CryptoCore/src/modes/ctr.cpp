#include "modes/ctr.h"

#include <stdexcept>
#include <algorithm>
#include "padding.h"

#include "aes_primitive.h"
#include <cstring>

namespace modes {
    std::vector<uint8_t> encrypt_ctr(const std::vector<uint8_t>& key, const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& nonce) {
        std::vector<uint8_t> ciphertext;
        ciphertext.reserve(plaintext.size());
        std::vector<uint8_t> counter = nonce;

        for (size_t i = 0; i < plaintext.size(); i += 16) {
            std::vector<uint8_t> keystream = aes_encrypt_block(key, counter);
            size_t block_size = std::min<size_t>(16, plaintext.size() - i);
            for (size_t j = 0; j < block_size; ++j) {
                ciphertext.push_back(plaintext[i + j] ^ keystream[j]);
            }
            
            for (int j = 15; j >= 0; --j) {
                if (++counter[j] != 0) break;
            }
        }
        return ciphertext;
    }

    std::vector<uint8_t> decrypt_ctr(const std::vector<uint8_t>& key, const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& nonce) {
        return encrypt_ctr(key, ciphertext, nonce); 
    }
}