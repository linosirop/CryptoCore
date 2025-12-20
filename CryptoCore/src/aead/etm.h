#pragma once

#include <vector>
#include <cstdint>

struct EtM_Result {
    std::vector<uint8_t> iv;
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> tag;
};

// Encrypt-then-MAC
EtM_Result etm_encrypt(
    const std::vector<uint8_t>& master_key,
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& iv
);

// returns false if authentication failed
bool etm_decrypt(
    const std::vector<uint8_t>& master_key,
    const std::vector<uint8_t>& iv,
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& tag,
    std::vector<uint8_t>& plaintext_out
);
