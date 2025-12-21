#pragma once
#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

// “ребуема€ по спринту сигнатура:
std::vector<uint8_t> derive_key(
    const std::vector<uint8_t>& master_key,
    const std::string& context,
    size_t length = 32
);

// ћожно оставить и внутреннее им€ (не об€зательно, но полезно)
std::vector<uint8_t> derive_key_hmac_sha256(
    const std::vector<uint8_t>& master_key,
    const std::string& context,
    size_t length
);
