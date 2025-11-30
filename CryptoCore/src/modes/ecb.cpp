#include "modes/ecb.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>

std::vector<uint8_t> hex_to_bytes(const std::string& hex) {  // CLI-3
    std::vector<uint8_t> bytes(hex.length() / 2);
    std::stringstream ss(hex);
    for (size_t i = 0; i < bytes.size(); ++i) {
        ss >> std::hex >> bytes[i];
    }
    return bytes;
}

namespace {
    std::vector<uint8_t> pkcs7_pad(const std::vector<uint8_t>& data, size_t block_size = 16) {  // CRY-4
        size_t pad_len = block_size - (data.size() % block_size);
        std::vector<uint8_t> padded = data;
        padded.resize(data.size() + pad_len, static_cast<uint8_t>(pad_len));
        return padded;
    }

    std::vector<uint8_t> pkcs7_unpad(const std::vector<uint8_t>& data) {  // CRY-4
        if (data.empty()) throw std::runtime_error("Invalid padding");
        uint8_t pad_len = data.back();
        if (pad_len > data.size() || pad_len == 0) throw std::runtime_error("Invalid padding");
        for (size_t i = 1; i <= pad_len; ++i) {
            if (data[data.size() - i] != pad_len) throw std::runtime_error("Invalid padding");
        }
        return {data.begin(), data.end() - pad_len};
    }
}

std::vector<uint8_t> ecb_encrypt(const std::vector<uint8_t>& key_bytes, const std::vector<uint8_t>& plaintext) {  // CRY-1, CRY-3
    if (key_bytes.size() != 16) throw std::invalid_argument("Key must be 16 bytes (CRY-1 AES-128)");
    auto padded = pkcs7_pad(plaintext, 16);  // CRY-4

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP init failed (CRY-2 OpenSSL)");

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, key_bytes.data(), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encrypt init failed");
    }

    std::vector<uint8_t> ciphertext(padded.size() + 16);
    int len, total_len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, padded.data(), static_cast<int>(padded.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encrypt update failed (CRY-3 block)");
    }
    total_len += len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + total_len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encrypt final failed");
    }
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(total_len);
    return ciphertext;
}

std::vector<uint8_t> ecb_decrypt(const std::vector<uint8_t>& key_bytes, const std::vector<uint8_t>& ciphertext) {  // CRY-1, CRY-3
    if (key_bytes.size() != 16) throw std::invalid_argument("Key must be 16 bytes");
    if (ciphertext.size() % 16 != 0) throw std::invalid_argument("Ciphertext not multiple of block size");

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP init failed");

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, key_bytes.data(), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decrypt init failed");
    }

    std::vector<uint8_t> plaintext(ciphertext.size());
    int len, total_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), static_cast<int>(ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decrypt update failed");
    }
    total_len += len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + total_len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decrypt final failed (padding error?)");
    }
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(total_len);
    return pkcs7_unpad(plaintext);  // CRY-4
}
