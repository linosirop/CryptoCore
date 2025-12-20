#include "etm.h"

#include "modes/ctr.h"
#include "mac/hmac.h"
#include "hash/sha256_stream.h"

#include <sstream>
#include <cstring>

// ===== key derivation =====
// enc_key = SHA256(master_key || 0x01)
// mac_key = SHA256(master_key || 0x02)

static std::vector<uint8_t> derive_key(
    const std::vector<uint8_t>& master,
    uint8_t suffix
) {
    SHA256_CTX ctx;
    sha256_init(ctx);
    sha256_update(ctx, master.data(), master.size());
    sha256_update(ctx, &suffix, 1);
    return sha256_final(ctx);
}

// helper: HMAC over raw buffer (ciphertext)
static std::vector<uint8_t> hmac_buffer(
    const std::vector<uint8_t>& key,
    const std::vector<uint8_t>& data
) {
    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    ss.write(reinterpret_cast<const char*>(data.data()), data.size());
    ss.seekg(0);
    return hmac_sha256(key, ss);
}

// ===== ENCRYPT =====
EtM_Result etm_encrypt(
    const std::vector<uint8_t>& master_key,
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& iv
) {
    EtM_Result res;
    res.iv = iv;

    auto enc_key = derive_key(master_key, 0x01);
    auto mac_key = derive_key(master_key, 0x02);

    // AES-CTR encryption
    res.ciphertext = modes::encrypt_ctr(enc_key, plaintext, iv);

    // HMAC over ciphertext
    res.tag = hmac_buffer(mac_key, res.ciphertext);

    return res;
}

// ===== DECRYPT =====
bool etm_decrypt(
    const std::vector<uint8_t>& master_key,
    const std::vector<uint8_t>& iv,
    const std::vector<uint8_t>& ciphertext,
    const std::vector<uint8_t>& tag,
    std::vector<uint8_t>& plaintext_out
) {
    auto enc_key = derive_key(master_key, 0x01);
    auto mac_key = derive_key(master_key, 0x02);

    // recompute tag
    auto expected_tag = hmac_buffer(mac_key, ciphertext);

    if (expected_tag.size() != tag.size())
        return false;

    // constant-time compare
    uint8_t diff = 0;
    for (size_t i = 0; i < tag.size(); ++i)
        diff |= expected_tag[i] ^ tag[i];

    if (diff != 0)
        return false;

    // decrypt only AFTER authentication
    plaintext_out = modes::decrypt_ctr(enc_key, ciphertext, iv);
    return true;
}
