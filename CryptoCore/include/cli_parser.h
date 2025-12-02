
#pragma once
#include <string>

struct CliArgs {
    std::string algorithm = "aes";  
    std::string mode = "ecb";
    bool encrypt = false;
    bool decrypt = false;
    std::string key_hex;  
    std::string input_file;
    std::string output_file;  
    std::string iv_hex;
};

CliArgs parse_args(int argc, char* argv[]);  
