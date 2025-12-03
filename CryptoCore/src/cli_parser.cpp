#include "cli_parser.h"
#include <iostream>
#include <map>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <algorithm>   
#include <set>          

CliArgs parse_args(int argc, char* argv[]) {
    CliArgs args;
    std::map<std::string, std::string> flags;

    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        
        if (arg.empty()) continue;

       
        if (arg.rfind("--", 0) == 0) {
            
            size_t eq_pos = arg.find('=');
            if (eq_pos != std::string::npos) {
               
                std::string key = arg.substr(2, eq_pos - 2);
                std::string value = arg.substr(eq_pos + 1);
                flags[key] = value;
            }
            else {
                
                std::string key = arg.substr(2);

                
                if (i + 1 < argc) {
                    std::string next_arg = argv[i + 1];
                    if (next_arg.rfind("--", 0) != 0) {
                        
                        flags[key] = next_arg;
                        i++; 
                    }
                    else {
                        
                        flags[key] = "true";
                    }
                }
                else {
                    
                    flags[key] = "true";
                }
            }
        }
        else {
            std::cerr << "[ERROR] Invalid arg format: " << arg << std::endl;
            std::cerr << "Use --key=value or --key value format" << std::endl;
            std::exit(1);
        }
    }

    
    if (flags.count("algorithm")) {
        args.algorithm = flags["algorithm"];
    }
    if (args.algorithm != "aes") {
        std::cerr << "[ERROR] Only 'aes' algorithm supported" << std::endl;
        std::exit(1);
    }

    
    if (flags.count("mode")) {
        args.mode = flags["mode"];

        
        std::transform(args.mode.begin(), args.mode.end(), args.mode.begin(),
            [](unsigned char c) { return std::tolower(c); });
    }

    const std::set<std::string> valid_modes = { "ecb", "cbc", "cfb", "ofb", "ctr" };
    if (valid_modes.count(args.mode) == 0) {
        std::cerr << "[ERROR] Invalid mode: " << args.mode << "\n"
            << "Supported modes: ecb, cbc, cfb, ofb, ctr" << std::endl;
        std::exit(1);
    }

    
    args.encrypt = flags.count("encrypt");
    args.decrypt = flags.count("decrypt");
    if ((args.encrypt && args.decrypt) || (!args.encrypt && !args.decrypt)) {
        std::cerr << "[ERROR] Specify exactly one: --encrypt or --decrypt" << std::endl;
        std::exit(1);
    }

    
    if (flags.count("key")) {
        args.key_hex = flags["key"];
        if (args.key_hex.length() != 32) {
            std::cerr << "[ERROR] --key must be 32 hex characters (16 bytes)" << std::endl;
            std::exit(1);
        }
        for (char c : args.key_hex) {
            if (!std::isxdigit(static_cast<unsigned char>(c))) {
                std::cerr << "[ERROR] --key must contain only hexadecimal characters" << std::endl;
                std::exit(1);
            }
        }
    }
    else {
        if (args.decrypt) {
            std::cerr << "[ERROR] --key is required for decryption" << std::endl;
            std::exit(1);
        }
    }

    
    if (flags.count("input")) {
        args.input_file = flags["input"];
    }
    else {
        std::cerr << "[ERROR] --input is required" << std::endl;
        std::exit(1);
    }

    
    if (flags.count("output")) {
        args.output_file = flags["output"];
    }
    else {
        
        args.output_file = args.input_file + (args.encrypt ? ".enc" : ".dec");
    }
    


    
    if (flags.count("iv")) {
        args.iv_hex = flags["iv"];
        // ѕри любом режиме сохран€ем IV; валидаци€ ниже
        if (args.iv_hex.length() != 32) {
            std::cerr << "[ERROR] --iv must be 32 hex characters (16 bytes)" << std::endl;
            std::exit(1);
        }
        for (char c : args.iv_hex) {
            if (!std::isxdigit(static_cast<unsigned char>(c))) {
                std::cerr << "[ERROR] --iv must contain only hexadecimal characters" << std::endl;
                std::exit(1);
            }
        }
    }


    if (flags.count("dump-random")) {
        // parse integer
        args.dump_random = std::stoul(flags["dump-random"]);
    }


    return args;
}