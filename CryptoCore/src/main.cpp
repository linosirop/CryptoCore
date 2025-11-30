#include "cli_parser.h"
#include "file_io.h"
#include "modes/ecb.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        CliArgs args = parse_args(argc, argv);  // CLI-1–5

        auto data = read_file(args.input_file);  // CRY-5
        auto key_bytes = hex_to_bytes(args.key_hex);  // CLI-3

        std::vector<uint8_t> result;
        if (args.encrypt) {
            result = ecb_encrypt(key_bytes, data);  // CRY-3/4
            std::cout << "[INFO] Шифрование завершено успешно." << std::endl;
        } else {
            result = ecb_decrypt(key_bytes, data);
            std::cout << "[INFO] Расшифровка завершена успешно." << std::endl;
        }

        write_file(args.output_file, result);  // CRY-5
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;  // CLI-4
    }
    return 0;
}
