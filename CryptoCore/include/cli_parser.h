#pragma once
#include <string>
#include <cstddef>

struct CliArgs {
    // dgst
    bool hmac = false;
    std::string verify_file;

    // common
    std::string command;
    std::string algorithm;   // для cipher: aes, для dgst: sha256/sha3-256, для derive: pbkdf2
    std::string mode;
    std::string key_hex;
    std::string iv_hex;
    std::string input_file;
    std::string output_file;
    std::string aad_hex;

    // AEAD
    bool etm = false;

    bool encrypt = false;
    bool decrypt = false;

    size_t dump_random = 0;

    // ===== Sprint-7: derive =====
    bool derive = false;
    std::string password;        // --password
    std::string password_file;   // --password-file (если сделаешь)
    std::string password_env;    // --password-env  (если сделаешь)
    std::string salt_hex;        // --salt (hex), если пусто -> генерируем 16 байт
    uint32_t iterations = 100000; // default
    size_t length = 32;           // default
    std::string derive_output_file; // --output (raw bytes)
};

CliArgs parse_args(int argc, char* argv[]);
