#pragma once
#include <vector>
#include <cstdint>

std::vector<uint8_t> pbkdf2_hmac_sha256(
    const std::vector<uint8_t>& password,
    const std::vector<uint8_t>& salt,
    uint32_t iterations,
    size_t dklen
);
