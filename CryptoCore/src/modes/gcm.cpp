#include "gcm.h"
#include "aes_primitive.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>

namespace modes {

    static const size_t BLOCK = 16;

    /* ================= GF(2^128) ================= */

    static void gf_mult(const uint8_t X[16], const uint8_t Y[16], uint8_t out[16]) {
        uint8_t Z[16] = { 0 };
        uint8_t V[16];
        memcpy(V, Y, 16);

        for (int i = 0; i < 128; ++i) {
            int xi = (X[i / 8] >> (7 - (i % 8))) & 1;
            if (xi) {
                for (int j = 0; j < 16; ++j)
                    Z[j] ^= V[j];
            }

            bool lsb = V[15] & 1;
            for (int j = 15; j > 0; --j)
                V[j] = (V[j] >> 1) | ((V[j - 1] & 1) << 7);
            V[0] >>= 1;
            if (lsb)
                V[0] ^= 0xE1;
        }

        memcpy(out, Z, 16);
    }

    /* ================= GHASH ================= */

    static std::vector<uint8_t> ghash(
        const std::vector<uint8_t>& H,
        const std::vector<uint8_t>& aad,
        const std::vector<uint8_t>& c
    ) {
        uint8_t Y[16] = { 0 };

        auto process = [&](const std::vector<uint8_t>& data) {
            for (size_t i = 0; i < data.size(); i += 16) {
                uint8_t block[16] = { 0 };
                size_t n = std::min<size_t>(16, data.size() - i);
                memcpy(block, data.data() + i, n);
                for (int j = 0; j < 16; ++j)
                    Y[j] ^= block[j];
                gf_mult(Y, H.data(), Y);
            }
            };

        process(aad);
        process(c);

        uint8_t len_block[16] = { 0 };
        uint64_t aad_bits = aad.size() * 8;
        uint64_t c_bits = c.size() * 8;

        for (int i = 0; i < 8; ++i) {
            len_block[7 - i] = (aad_bits >> (i * 8)) & 0xff;
            len_block[15 - i] = (c_bits >> (i * 8)) & 0xff;
        }

        for (int j = 0; j < 16; ++j)
            Y[j] ^= len_block[j];

        gf_mult(Y, H.data(), Y);

        return std::vector<uint8_t>(Y, Y + 16);
    }

    /* ================= CTR ================= */

    static void inc32(uint8_t counter[16]) {
        for (int i = 15; i >= 12; --i)
            if (++counter[i]) break;
    }

    /* ================= ENCRYPT ================= */

    GCM_Result gcm_encrypt(
        const std::vector<uint8_t>& key,
        const std::vector<uint8_t>& plaintext,
        const std::vector<uint8_t>& aad,
        const std::vector<uint8_t>& nonce
    ) {
        if (nonce.size() != 12)
            throw std::runtime_error("GCM nonce must be 12 bytes");

        uint8_t H_raw[16] = { 0 };
        std::vector<uint8_t> H = aes_encrypt_block(key, std::vector<uint8_t>(H_raw, H_raw + 16));

        uint8_t J0_raw[16] = { 0 };
        memcpy(J0_raw, nonce.data(), 12);
        J0_raw[15] = 1;
        std::vector<uint8_t> J0(J0_raw, J0_raw + 16);

        std::vector<uint8_t> ciphertext(plaintext.size());
        uint8_t ctr_raw[16];
        memcpy(ctr_raw, J0_raw, 16);

        for (size_t i = 0; i < plaintext.size(); i += 16) {
            inc32(ctr_raw);
            std::vector<uint8_t> ctr(ctr_raw, ctr_raw + 16);
            auto ks = aes_encrypt_block(key, ctr);

            size_t n = std::min<size_t>(16, plaintext.size() - i);
            for (size_t j = 0; j < n; ++j)
                ciphertext[i + j] = plaintext[i + j] ^ ks[j];
        }

        auto S = ghash(H, aad, ciphertext);
        auto E = aes_encrypt_block(key, J0);

        std::vector<uint8_t> tag(16);
        for (int i = 0; i < 16; ++i)
            tag[i] = S[i] ^ E[i];

        return { nonce, ciphertext, tag };
    }

    /* ================= DECRYPT ================= */

    bool gcm_decrypt(
        const std::vector<uint8_t>& key,
        const std::vector<uint8_t>& nonce,
        const std::vector<uint8_t>& ciphertext,
        const std::vector<uint8_t>& aad,
        const std::vector<uint8_t>& tag,
        std::vector<uint8_t>& plaintext_out
    ) {
        uint8_t H_raw[16] = { 0 };
        std::vector<uint8_t> H = aes_encrypt_block(key, std::vector<uint8_t>(H_raw, H_raw + 16));

        uint8_t J0_raw[16] = { 0 };
        memcpy(J0_raw, nonce.data(), 12);
        J0_raw[15] = 1;
        std::vector<uint8_t> J0(J0_raw, J0_raw + 16);

        auto S = ghash(H, aad, ciphertext);
        auto E = aes_encrypt_block(key, J0);

        uint8_t diff = 0;
        for (int i = 0; i < 16; ++i)
            diff |= (S[i] ^ E[i]) ^ tag[i];

        if (diff != 0)
            return false;

        plaintext_out.resize(ciphertext.size());
        uint8_t ctr_raw[16];
        memcpy(ctr_raw, J0_raw, 16);

        for (size_t i = 0; i < ciphertext.size(); i += 16) {
            inc32(ctr_raw);
            std::vector<uint8_t> ctr(ctr_raw, ctr_raw + 16);
            auto ks = aes_encrypt_block(key, ctr);

            size_t n = std::min<size_t>(16, ciphertext.size() - i);
            for (size_t j = 0; j < n; ++j)
                plaintext_out[i + j] = ciphertext[i + j] ^ ks[j];
        }

        return true;
    }

} // namespace modes
