#ifndef SHA3_STREAM_H
#define SHA3_STREAM_H

#include <cstdint>
#include <vector>
#include <cstddef>

struct SHA3_CTX {
    uint64_t state[25];
    uint8_t buffer[136];      
    size_t buffer_len;
};

void sha3_256_init(SHA3_CTX& ctx);
void sha3_256_update(SHA3_CTX& ctx, const uint8_t* data, size_t len);
std::vector<uint8_t> sha3_256_final(SHA3_CTX& ctx);

#endif
