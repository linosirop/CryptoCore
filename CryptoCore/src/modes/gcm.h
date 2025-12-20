#pragma once
#include <vector>
#include <cstdint>

namespace modes {

struct GCM_Result {
    std::vector<uint8_t> nonce;      // 12 bytes
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> tag;        // 16 bytes
};

// Encrypt: returns nonce || ciphertext || tag (separately)
GCM_Result gcm_encrypt(
    const std::vector<uint8_t>& key,
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& aad,
    const std::vector<uint8_t>& nonce   // must be 12 bytes
);

// Decrypt: returns false if authentication failed
bool gcm_decrypt(
    const std::vector<uint8_t>& key,
    const std::vector<uint8_t>& nonce,
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& aad,
    const std::vector<uint8_t>& tag,
    std::vector<uint8_t>& plaintext_out
);

}
