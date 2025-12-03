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
#include <stdexcept>

static std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    if (hex.size() % 2 != 0) {
        throw std::runtime_error("hex_to_bytes: odd-length hex string");
    }
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);

    try {
        for (size_t i = 0; i < hex.size(); i += 2) {
            out.push_back(static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16)));
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("hex_to_bytes: invalid hex: ") + e.what());
    }
    return out;
}

static void print_hex(const std::string& label, const std::vector<uint8_t>& buf) {
    std::cout << label;
    std::cout << std::hex << std::setfill('0');
    for (uint8_t b : buf) std::cout << std::setw(2) << (int)b;
    std::cout << std::dec << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        CliArgs args = parse_args(argc, argv);

        // Random dump for NIST tests — Sprint 3 feature
        if (args.dump_random > 0) {
            auto rnd = generate_random_bytes(args.dump_random);
            write_file(args.output_file.empty() ? "random.bin" : args.output_file, rnd);
            std::cout << "[INFO] Wrote " << rnd.size() << " random bytes." << std::endl;
            return 0;
        }

        bool needs_iv = (args.mode != "ecb");
        std::vector<uint8_t> key;
        std::vector<uint8_t> iv(16);

        // ==========================================================
        // ENCRYPTION
        // ==========================================================
        if (args.encrypt) {
            std::vector<uint8_t> plaintext = read_file(args.input_file);

            // -------------------- KEY HANDLING --------------------
            if (args.key_hex.empty()) {
                key = generate_random_bytes(16);
                print_hex("[INFO] Generated random key: ", key);
            }
            else {
                key = hex_to_bytes(args.key_hex);

                if (key.size() != 16) {
                    std::cerr << "[ERROR] AES-128 key must be 16 bytes." << std::endl;
                    return 1;
                }

                bool zero = true;
                bool seq = true;

                for (size_t i = 0; i < 16; i++) {
                    if (key[i] != 0) zero = false;
                    if (i > 0 && key[i] != uint8_t(key[i - 1] + 1)) seq = false;
                }

                if (zero) {
                    std::cerr << "[WEAK-KEY-ZERO]" << std::endl;
                }
                if (seq) {
                    std::cerr << "[WEAK-KEY-SEQ]" << std::endl;
                }

            }

            // -------------------- IV HANDLING --------------------
            if (needs_iv) {
                if (!args.iv_hex.empty()) {
                    iv = hex_to_bytes(args.iv_hex);
                    if (iv.size() != 16) {
                        std::cerr << "[ERROR] IV must be 16 bytes." << std::endl;
                        return 1;
                    }
                    print_hex("[INFO] Using IV from CLI: ", iv);
                }
                else {
                    iv = generate_random_bytes(16);
                    print_hex("[INFO] Generated IV: ", iv);
                }
            }

            // -------------------- ENCRYPT --------------------
            std::vector<uint8_t> ciphertext;

            if (args.mode == "ecb") ciphertext = ecb_encrypt(key, plaintext);
            else if (args.mode == "cbc") ciphertext = modes::encrypt_cbc(key, plaintext, iv);
            else if (args.mode == "cfb") ciphertext = modes::encrypt_cfb(key, plaintext, iv);
            else if (args.mode == "ofb") ciphertext = modes::encrypt_ofb(key, plaintext, iv);
            else if (args.mode == "ctr") ciphertext = modes::encrypt_ctr(key, plaintext, iv);

            // -------------------- WRITE --------------------
            if (needs_iv && args.iv_hex.empty())
                write_file_with_iv(args.output_file, iv, ciphertext);
            else
                write_file(args.output_file, ciphertext);

            std::cout << "[INFO] Encryption completed." << std::endl;
            return 0;
        }

        // ==========================================================
        // DECRYPTION
        // ==========================================================
        if (!args.encrypt) {
            if (args.key_hex.empty()) {
                std::cerr << "[ERROR] --key is required for decryption." << std::endl;
                return 1;
            }

            key = hex_to_bytes(args.key_hex);
            if (key.size() != 16) {
                std::cerr << "[ERROR] AES-128 key must be 16 bytes." << std::endl;
                return 1;
            }

            std::vector<uint8_t> encdata;

            if (needs_iv) {
                if (args.iv_hex.empty()) {
                    encdata = read_file_with_iv(args.input_file, iv);
                }
                else {
                    iv = hex_to_bytes(args.iv_hex);
                    if (iv.size() != 16) {
                        std::cerr << "[ERROR] IV must be 16 bytes." << std::endl;
                        return 1;
                    }
                    encdata = read_file(args.input_file);
                }
            }
            else {
                encdata = read_file(args.input_file);
            }

            std::vector<uint8_t> decrypted;
            if (args.mode == "ecb") decrypted = ecb_decrypt(key, encdata);
            else if (args.mode == "cbc") decrypted = modes::decrypt_cbc(key, encdata, iv);
            else if (args.mode == "cfb") decrypted = modes::decrypt_cfb(key, encdata, iv);
            else if (args.mode == "ofb") decrypted = modes::decrypt_ofb(key, encdata, iv);
            else if (args.mode == "ctr") decrypted = modes::decrypt_ctr(key, encdata, iv);

            write_file(args.output_file, decrypted);
            std::cout << "[INFO] Decryption completed." << std::endl;
            return 0;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
