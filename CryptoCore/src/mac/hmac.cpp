#include "hmac.h"
#include <algorithm>

static const size_t BLOCK_SIZE = 64;

/* ================= INIT ================= */

void hmac_init(
    HMAC_CTX& ctx,
    const std::vector<uint8_t>& key
) {
    std::vector<uint8_t> k = key;

    // Key normalization
    if (k.size() > BLOCK_SIZE) {
        SHA256_CTX kctx;
        sha256_init(kctx);
        sha256_update(kctx, k.data(), k.size());
        k = sha256_final(kctx);
    }

    k.resize(BLOCK_SIZE, 0x00);

    std::vector<uint8_t> ipad(BLOCK_SIZE);
    std::vector<uint8_t> opad(BLOCK_SIZE);

    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        ipad[i] = k[i] ^ 0x36;
        opad[i] = k[i] ^ 0x5c;
    }

    sha256_init(ctx.inner);
    sha256_update(ctx.inner, ipad.data(), ipad.size());

    sha256_init(ctx.outer);
    sha256_update(ctx.outer, opad.data(), opad.size());
}

/* ================= UPDATE ================= */

void hmac_update(
    HMAC_CTX& ctx,
    const uint8_t* data,
    size_t len
) {
    sha256_update(ctx.inner, data, len);
}

/* ================= FINAL ================= */

std::vector<uint8_t> hmac_final(
    HMAC_CTX& ctx
) {
    auto inner_hash = sha256_final(ctx.inner);
    sha256_update(ctx.outer, inner_hash.data(), inner_hash.size());
    return sha256_final(ctx.outer);
}

/* ================= LEGACY STREAM API ================= */

std::vector<uint8_t> hmac_sha256(
    const std::vector<uint8_t>& key,
    std::istream& in
) {
    HMAC_CTX ctx;
    hmac_init(ctx, key);

    const size_t CHUNK = 64 * 1024;
    std::vector<uint8_t> buf(CHUNK);

    while (true) {
        in.read(reinterpret_cast<char*>(buf.data()), buf.size());
        std::streamsize r = in.gcount();
        if (r <= 0) break;
        hmac_update(ctx, buf.data(), (size_t)r);
    }

    return hmac_final(ctx);
}
