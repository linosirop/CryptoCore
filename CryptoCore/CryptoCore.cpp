#include "cli_parser.h"
#include "file_io.h"
#include "modes/ecb.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        CliArgs args = parse_args(argc, argv);

        auto data = read_file(args.input_file);
        auto key_bytes = hex_to_bytes(args.key_hex);

        std::vector<uint8_t> result;
        if (args.encrypt) {
            result = ecb_encrypt(key_bytes, data);
            std::cout << "[INFO] Encryption completed successfully." << std::endl;
        }
        else {
            result = ecb_decrypt(key_bytes, data);
            std::cout << "[INFO] Decryption completed successfully." << std::endl;
        }

        write_file(args.output_file, result);
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}