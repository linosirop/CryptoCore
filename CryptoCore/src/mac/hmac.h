#pragma once
#include <vector>
#include <cstdint>
#include <istream>

#include "C:\Users\User\source\repos\CryptoCore\CryptoCore\src\hash\sha256_stream.h"

struct HMAC_CTX {
    SHA256_CTX inner;
    SHA256_CTX outer;
};

void hmac_init(
    HMAC_CTX& ctx,
    const std::vector<uint8_t>& key
);

void hmac_update(
    HMAC_CTX& ctx,
    const uint8_t* data,
    size_t len
);

std::vector<uint8_t> hmac_final(
    HMAC_CTX& ctx
);

// —тарый интерфейс Ч ќ—“ј¬Ћя≈ћ дл€ dgst
std::vector<uint8_t> hmac_sha256(
    const std::vector<uint8_t>& key,
    std::istream& in
);
