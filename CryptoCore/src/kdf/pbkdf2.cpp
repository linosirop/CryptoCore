#include "pbkdf2.h"
#include "../mac/hmac.h"

#include <vector>
#include <cstdint>
#include <cstring>

/*
 * PBKDF2-HMAC-SHA256 (RFC 2898)
 */

static std::vector<uint8_t> hmac_sha256_bytes(
    const std::vector<uint8_t>& key,
    const std::vector<uint8_t>& data
) {
    HMAC_CTX ctx;
    hmac_init(ctx, key);
    hmac_update(ctx, data.data(), data.size());
    return hmac_final(ctx);
}

std::vector<uint8_t> pbkdf2_hmac_sha256(
    const std::vector<uint8_t>& password,
    const std::vector<uint8_t>& salt,
    uint32_t iterations,
    size_t dklen
) {
    constexpr size_t HLEN = 32; // SHA-256 output
    size_t blocks = (dklen + HLEN - 1) / HLEN;

    std::vector<uint8_t> derived;
    derived.reserve(blocks * HLEN);

    for (uint32_t i = 1; i <= blocks; ++i) {
        // salt || INT_32_BE(i)
        std::vector<uint8_t> block_input = salt;
        block_input.push_back((i >> 24) & 0xff);
        block_input.push_back((i >> 16) & 0xff);
        block_input.push_back((i >> 8) & 0xff);
        block_input.push_back(i & 0xff);

        // U1
        std::vector<uint8_t> u = hmac_sha256_bytes(password, block_input);
        std::vector<uint8_t> t = u;

        // U2 .. Uc
        for (uint32_t j = 2; j <= iterations; ++j) {
            u = hmac_sha256_bytes(password, u);
            for (size_t k = 0; k < HLEN; ++k)
                t[k] ^= u[k];
        }

        derived.insert(derived.end(), t.begin(), t.end());
    }

    derived.resize(dklen);
    return derived;
}
