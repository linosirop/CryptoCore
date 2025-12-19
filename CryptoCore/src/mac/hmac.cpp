#include "hmac.h"
#include "C:\Users\User\source\repos\CryptoCore\CryptoCore\src\hash\sha256_stream.h"
#include <vector>
#include <cstdint>
#include <algorithm>

static const size_t BLOCK_SIZE = 64;

std::vector<uint8_t> hmac_sha256(
    const std::vector<uint8_t>& key,
    std::istream& in
) {
    std::vector<uint8_t> k = key;

    // Step 1: key normalization
    if (k.size() > BLOCK_SIZE) {
        SHA256_CTX kctx;
        sha256_init(kctx);
        sha256_update(kctx, k.data(), k.size());
        k = sha256_final(kctx);
    }

    k.resize(BLOCK_SIZE, 0x00);

    // Step 2: ipad / opad
    std::vector<uint8_t> ipad(BLOCK_SIZE);
    std::vector<uint8_t> opad(BLOCK_SIZE);

    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        ipad[i] = k[i] ^ 0x36;
        opad[i] = k[i] ^ 0x5c;
    }

    // Step 3: inner hash
    SHA256_CTX inner;
    sha256_init(inner);
    sha256_update(inner, ipad.data(), ipad.size());

    const size_t CHUNK = 64 * 1024;
    std::vector<uint8_t> buf(CHUNK);

    while (true) {
        in.read(reinterpret_cast<char*>(buf.data()), buf.size());
        std::streamsize r = in.gcount();
        if (r <= 0) break;
        sha256_update(inner, buf.data(), static_cast<size_t>(r));
    }

    auto inner_hash = sha256_final(inner);

    // Step 4: outer hash
    SHA256_CTX outer;
    sha256_init(outer);
    sha256_update(outer, opad.data(), opad.size());
    sha256_update(outer, inner_hash.data(), inner_hash.size());

    return sha256_final(outer);
}
