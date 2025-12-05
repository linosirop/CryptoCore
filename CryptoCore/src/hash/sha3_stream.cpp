
#include "sha3_stream.h"
#include <cstring>
#include <cstdint>

static inline uint64_t rotl64(uint64_t x, unsigned r) {
    r &= 63u; 
    if (r == 0) return x;
    return (x << r) | (x >> (64 - r));
}

static void keccakf(uint64_t st[25]) {
    static const uint64_t RC[24] = {
        0x0000000000000001ULL,0x0000000000008082ULL,
        0x800000000000808aULL,0x8000000080008000ULL,
        0x000000000000808bULL,0x0000000080000001ULL,
        0x8000000080008081ULL,0x8000000000008009ULL,
        0x000000000000008aULL,0x0000000000000088ULL,
        0x0000000080008009ULL,0x000000008000000aULL,
        0x000000008000808bULL,0x800000000000008bULL,
        0x8000000000008089ULL,0x8000000000008003ULL,
        0x8000000000008002ULL,0x8000000000000080ULL,
        0x000000000000800aULL,0x800000008000000aULL,
        0x8000000080008081ULL,0x8000000000008080ULL,
        0x0000000080000001ULL,0x8000000080008008ULL
    };

    static const unsigned r[5][5] = {
        {  0, 36,  3, 41, 18 },
        {  1, 44, 10, 45,  2 },
        { 62,  6, 43, 15, 61 },
        { 28, 55, 25, 21, 56 },
        { 27, 20, 39,  8, 14 }
    };

    for (int round = 0; round < 24; ++round) {
        uint64_t C[5], D[5], B[25];

        for (int x = 0; x < 5; ++x)
            C[x] = st[x] ^ st[x + 5] ^ st[x + 10] ^ st[x + 15] ^ st[x + 20];

        for (int x = 0; x < 5; ++x)
            D[x] = C[(x + 4) % 5] ^ rotl64(C[(x + 1) % 5], 1);

        for (int x = 0; x < 5; ++x)
            for (int y = 0; y < 5; ++y)
                st[x + 5 * y] ^= D[x];

        for (int x = 0; x < 5; ++x)
            for (int y = 0; y < 5; ++y)
                B[y + 5 * ((2 * x + 3 * y) % 5)] = rotl64(st[x + 5 * y], r[x][y]);

        for (int x = 0; x < 5; ++x)
            for (int y = 0; y < 5; ++y)
                st[x + 5 * y] = B[x + 5 * y] ^ ((~B[((x + 1) % 5) + 5 * y]) & B[((x + 2) % 5) + 5 * y]);

        st[0] ^= RC[round];
    }
}

void sha3_256_init(SHA3_CTX& ctx) {
    // state: 25 lanes * 8 = 200 bytes
    std::memset(ctx.state, 0, sizeof(ctx.state));
    ctx.buffer_len = 0;
    std::memset(ctx.buffer, 0, sizeof(ctx.buffer));
}

void sha3_256_update(SHA3_CTX& ctx, const uint8_t* data, size_t len) {
    const size_t rate = 136;

    size_t idx = 0;

    while (idx < len) {
        size_t need = rate - ctx.buffer_len;
        size_t take = (len - idx < need) ? (len - idx) : need;
        std::memcpy(ctx.buffer + ctx.buffer_len, data + idx, take);
        ctx.buffer_len += take;
        idx += take;

        if (ctx.buffer_len == rate) {
            // XOR block into the state (little-endian lanes)
            for (size_t i = 0; i < rate / 8; i++) {
                uint64_t lane = 0;
                for (int b = 0; b < 8; b++)
                    lane |= uint64_t(ctx.buffer[i * 8 + b]) << (8 * b);
                ctx.state[i] ^= lane;
            }
            keccakf(ctx.state);
            ctx.buffer_len = 0;
        }
    }
}

std::vector<uint8_t> sha3_256_final(SHA3_CTX& ctx) {
    const size_t rate = 136;
    const size_t outlen = 32;

    // domain separation 0x06 and pad10*1
    ctx.buffer[ctx.buffer_len++] = 0x06;

    // pad with zeros until last byte
    while (ctx.buffer_len < rate - 1)
        ctx.buffer[ctx.buffer_len++] = 0x00;

    // set final bit
    ctx.buffer[rate - 1] |= 0x80;

    // absorb final block
    for (size_t i = 0; i < rate / 8; i++) {
        uint64_t lane = 0;
        for (int b = 0; b < 8; b++)
            lane |= uint64_t(ctx.buffer[i * 8 + b]) << (8 * b);
        ctx.state[i] ^= lane;
    }
    keccakf(ctx.state);

    std::vector<uint8_t> out;
    out.reserve(outlen);

    size_t produced = 0;
    while (produced < outlen) {
        for (size_t i = 0; i < rate / 8 && produced < outlen; i++) {
            uint64_t lane = ctx.state[i];
            for (int b = 0; b < 8 && produced < outlen; b++) {
                out.push_back(static_cast<uint8_t>((lane >> (8 * b)) & 0xFF));
                produced++;
            }
        }
        if (produced < outlen)
            keccakf(ctx.state);
    }

    return out;
}
