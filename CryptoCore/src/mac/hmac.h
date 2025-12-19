#pragma once
#include <vector>
#include <cstdint>
#include "C:\Users\User\source\repos\CryptoCore\CryptoCore\src\hash\sha256_stream.h"
#include <istream>

std::vector<uint8_t> hmac_sha256(
    const std::vector<uint8_t>& key,
    std::istream& in
);
