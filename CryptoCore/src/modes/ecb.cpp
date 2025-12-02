#define OPENSSL_SUPPRESS_DEPRECATED
#include "modes/ecb.h"
#include "padding.h"
#include <openssl/aes.h>
#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <iostream>

std::vector<uint8_t> ecb_encrypt(const std::vector<uint8_t>& key, const std::vector<uint8_t>& plaintext) {
    auto padded = pkcs7_pad(plaintext, 16);  

    AES_KEY aes_key;
    if (AES_set_encrypt_key(key.data(), 128, &aes_key) < 0) {
        throw std::runtime_error("AES_set_encrypt_key failed");
    }

    std::vector<uint8_t> ciphertext(padded.size());
    for (size_t i = 0; i < padded.size(); i += 16) {
        AES_encrypt(padded.data() + i, ciphertext.data() + i, &aes_key);
    }
    return ciphertext;
}

std::vector<uint8_t> ecb_decrypt(const std::vector<uint8_t>& key, const std::vector<uint8_t>& ciphertext) {
    
    if (ciphertext.size() % 16 != 0) {
        throw std::runtime_error("Invalid ciphertext size for ECB");
    }
    if (ciphertext.empty()) {
        throw std::runtime_error("Empty ciphertext");
    }

    AES_KEY aes_key;
    if (AES_set_decrypt_key(key.data(), 128, &aes_key) < 0) {
        throw std::runtime_error("AES_set_decrypt_key failed");
    }

    std::vector<uint8_t> padded(ciphertext.size());
    for (size_t i = 0; i < ciphertext.size(); i += 16) {
        AES_decrypt(ciphertext.data() + i, padded.data() + i, &aes_key);
    }
    std::ofstream dbg("padded_ecb.bin", std::ios::binary);
    if (dbg) {
        dbg.write(reinterpret_cast<const char*>(padded.data()), static_cast<std::streamsize>(padded.size()));
        dbg.close();
    }
    else {
        std::cerr << "[DEBUG] Cannot open padded_ecb.bin for writing\n";
    }



    return pkcs7_unpad(padded);
}