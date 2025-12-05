#include "cli_parser.h"
#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <cstdlib>

CliArgs parse_args(int argc, char* argv[]) {
    CliArgs args;
    std::map<std::string, std::string> flags;

    
    // Detect "dgst" command (hashing mode)
    
    int index = 1;
    if (argc > 1 && std::string(argv[1]) == "dgst") {
        args.command = "dgst";
        index = 2;
    }

    
    // Parse flags: --key value   or  --flag
   
    for (int i = index; i < argc; i++) {
        std::string a = argv[i];

        if (a.rfind("--", 0) == 0) {
            std::string key = a.substr(2);
            std::string value = "true";

            if (i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) != 0) {
                value = argv[i + 1];
                i++;
            }

            flags[key] = value;
        }
        else {
            std::cerr << "[ERROR] Invalid arg: " << a << "\n";
            exit(1);
        }
    }

    
    // HASH MODE (dgst)
    
    if (args.command == "dgst") {
        if (!flags.count("algorithm")) {
            std::cerr << "[ERROR] dgst requires --algorithm\n";
            exit(1);
        }

        args.algorithm = flags["algorithm"];
        std::transform(args.algorithm.begin(), args.algorithm.end(),
            args.algorithm.begin(), ::tolower);

        if (args.algorithm != "sha256" && args.algorithm != "sha3-256") {
            std::cerr << "[ERROR] Supported hash algorithms: sha256, sha3-256\n";
            exit(1);
        }

        if (!flags.count("input")) {
            std::cerr << "[ERROR] dgst requires --input\n";
            exit(1);
        }

        args.input_file = flags["input"];
        args.output_file = flags.count("output") ? flags["output"] : "hash.bin";

        // NOTHING ELSE REQUIRED — EXIT HERE
        return args;
    }

    
    // CIPHER MODE (AES)
    
    if (!flags.count("algorithm")) {
        std::cerr << "[ERROR] Missing --algorithm\n";
        exit(1);
    }

    args.algorithm = flags["algorithm"];
    std::transform(args.algorithm.begin(), args.algorithm.end(),
        args.algorithm.begin(), ::tolower);

    if (args.algorithm != "aes") {
        std::cerr << "[ERROR] Only AES supported in cipher mode\n";
        exit(1);
    }

    //  mode 
    if (!flags.count("mode")) {
        std::cerr << "[ERROR] Missing --mode\n";
        exit(1);
    }
    args.mode = flags["mode"];
    std::transform(args.mode.begin(), args.mode.end(),
        args.mode.begin(), ::tolower);

    const std::set<std::string> valid_modes = {
        "ecb", "cbc", "cfb", "ofb", "ctr"
    };

    if (!valid_modes.count(args.mode)) {
        std::cerr << "[ERROR] Invalid mode\n";
        exit(1);
    }

    //  encrypt / decrypt 
    args.encrypt = flags.count("encrypt");
    args.decrypt = flags.count("decrypt");

    if (args.encrypt == args.decrypt) {
        std::cerr << "[ERROR] Use exactly one: --encrypt or --decrypt\n";
        exit(1);
    }

    //  input 
    if (!flags.count("input")) {
        std::cerr << "[ERROR] --input required\n";
        exit(1);
    }
    args.input_file = flags["input"];

    //  output 
    if (flags.count("output"))
        args.output_file = flags["output"];
    else
        args.output_file = args.input_file + (args.encrypt ? ".enc" : ".dec");

    //  key / iv / random 
    if (flags.count("key"))
        args.key_hex = flags["key"];

    if (flags.count("iv"))
        args.iv_hex = flags["iv"];

    if (flags.count("dump-random"))
        args.dump_random = std::stoul(flags["dump-random"]);

    return args;
}
