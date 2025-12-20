#pragma once
#include <string>

struct CliArgs {
    bool hmac = false;
    std::string verify_file;

    std::string command;
    std::string algorithm;
    std::string mode;
    std::string key_hex;
    std::string iv_hex;
    std::string input_file;
    std::string output_file;
    std::string aad_hex;

    // Encrypt-then-MAC
    bool etm = false;

    bool encrypt = false;
    bool decrypt = false;

    size_t dump_random = 0;
};

CliArgs parse_args(int argc, char* argv[]);
