#ifndef SHA256_STREAM_H
#define SHA256_STREAM_H

#include <cstddef>
#include <cstdint>
#include <vector>

struct SHA256_CTX {
    uint32_t state[8];
    uint64_t bitlen;
    uint8_t buffer[64];
    size_t buffer_len;
};

void sha256_init(SHA256_CTX& ctx);
void sha256_update(SHA256_CTX& ctx, const uint8_t* data, size_t len);
std::vector<uint8_t> sha256_final(SHA256_CTX& ctx);

#endif
