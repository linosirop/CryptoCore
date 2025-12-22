#include "hkdf.h"
#include "hmac.h"

#include <vector>
#include <string>
#include <cstdint>

static std::vector<uint8_t> hmac_sha256_bytes(
    const std::vector<uint8_t>& key,
    const std::vector<uint8_t>& data
) {
    HMAC_CTX ctx;
    hmac_init(ctx, key);
    hmac_update(ctx, data.data(), data.size());
    return hmac_final(ctx);
}

std::vector<uint8_t> derive_key_hmac_sha256(
    const std::vector<uint8_t>& master_key,
    const std::string& context,
    size_t length
) {
    std::vector<uint8_t> ctx_bytes(context.begin(), context.end());

    std::vector<uint8_t> out;
    out.reserve(length);

    uint32_t counter = 1;
    while (out.size() < length) {
        std::vector<uint8_t> msg = ctx_bytes;
        msg.push_back(uint8_t((counter >> 24) & 0xff));
        msg.push_back(uint8_t((counter >> 16) & 0xff));
        msg.push_back(uint8_t((counter >> 8) & 0xff));
        msg.push_back(uint8_t(counter & 0xff));

        auto block = hmac_sha256_bytes(master_key, msg);

        size_t need = length - out.size();
        size_t take = (need < block.size()) ? need : block.size();
        out.insert(out.end(), block.begin(), block.begin() + take);

        counter++;
    }

    return out;
}

// ✅ ВОТ ЭТОГО НЕ ХВАТАЛО: имя, которое ждёт тест
std::vector<uint8_t> derive_key(
    const std::vector<uint8_t>& master_key,
    const std::string& context,
    size_t length
) {
    return derive_key_hmac_sha256(master_key, context, length);
}
