
#pragma once
#include <string>

struct CliArgs {
    std::string algorithm = "aes";  // CLI-2
    std::string mode = "ecb";
    bool encrypt = false;
    bool decrypt = false;
    std::string key_hex;  // CLI-3: hex, 16 bytes
    std::string input_file;
    std::string output_file;  // CLI-5: default
};

CliArgs parse_args(int argc, char* argv[]);  // CLI-4: validate
