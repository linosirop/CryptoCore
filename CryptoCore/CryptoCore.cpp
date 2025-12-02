#include "cli_parser.h"
#include "file_io.h"
#include "csprng.h"
#include "padding.h"           
#include "modes/ecb.h"
#include "modes/cbc.h"
#include "modes/cfb.h"
#include "modes/ofb.h"
#include "modes/ctr.h"
#include <iostream>
#include <iomanip>

static std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

int main(int argc, char* argv[]) {
    try {
        CliArgs args = parse_args(argc, argv);

        std::vector<uint8_t> key_bytes = hex_to_bytes(args.key_hex);
        std::vector<uint8_t> iv(16);
        std::vector<uint8_t> data, result;

        bool needs_iv = (args.mode != "ecb");
        
        bool needs_padding = false;

        if (args.encrypt) {
            data = read_file(args.input_file);

            if (needs_iv) {
                if (!args.iv_hex.empty()) {
                    iv = hex_to_bytes(args.iv_hex);
                    std::cout << "[INFO] Using IV from CLI: " << args.iv_hex << std::endl;
                }
                else {
                    iv = generate_random_bytes(16);
                    std::cout << "[INFO] Generated IV: ";
                    for (uint8_t b : iv)
                        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
                    std::cout << std::endl;
                }
            }

            if (args.mode == "ecb") result = ecb_encrypt(key_bytes, data);
            else if (args.mode == "cbc") result = modes::encrypt_cbc(key_bytes, data, iv);
            else if (args.mode == "cfb") result = modes::encrypt_cfb(key_bytes, data, iv);
            else if (args.mode == "ofb") result = modes::encrypt_ofb(key_bytes, data, iv);
            else if (args.mode == "ctr") result = modes::encrypt_ctr(key_bytes, data, iv);

            if (needs_iv) {

                if (!args.iv_hex.empty()) {
                    write_file(args.output_file, result); 
                }
                else {
                    
                    write_file_with_iv(args.output_file, iv, result);
                }
            }
            else {
                write_file(args.output_file, result);
            }

            std::cout << "[INFO] Encryption completed." << std::endl;
        }
        else {
            if (args.mode == "ecb") {
                data = read_file(args.input_file);
            }
            else {
                // Для CBC/CFB/OFB/CTR
                if (args.iv_hex.empty()) {
                    data = read_file_with_iv(args.input_file, iv);
                }
                else {
                    iv = hex_to_bytes(args.iv_hex);
                    data = read_file(args.input_file);
                }
            }

            // расшифровываем
            if (args.mode == "ecb") result = ecb_decrypt(key_bytes, data);
            else if (args.mode == "cbc") result = modes::decrypt_cbc(key_bytes, data, iv);
            else if (args.mode == "cfb") result = modes::decrypt_cfb(key_bytes, data, iv);
            else if (args.mode == "ofb") result = modes::decrypt_ofb(key_bytes, data, iv);
            else if (args.mode == "ctr") result = modes::decrypt_ctr(key_bytes, data, iv);

            write_file(args.output_file, result);
            std::cout << "[INFO] Decryption completed." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}