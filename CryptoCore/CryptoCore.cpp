#include "cli_parser.h"
#include "file_io.h"
#include "csprng.h"
#include "padding.h"

#include "modes/ecb.h"
#include "modes/cbc.h"
#include "modes/cfb.h"
#include "modes/ofb.h"
#include "modes/ctr.h"
#include "modes/gcm.h"

#include "sha256_stream.h"
#include "sha3_stream.h"
#include "hmac.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

static std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    if (hex.size() % 2 != 0) {
        std::cerr << "[ERROR] Hex string length must be even\n";
        exit(1);
    }

    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);

    auto hex_value = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
        };

    for (size_t i = 0; i < hex.size(); i += 2) {
        int hi = hex_value(hex[i]);
        int lo = hex_value(hex[i + 1]);
        if (hi < 0 || lo < 0) {
            std::cerr << "[ERROR] Invalid hex string\n";
            exit(1);
        }
        out.push_back(uint8_t((hi << 4) | lo));
    }
    return out;
}

int main(int argc, char* argv[]) {
    try {
        CliArgs args = parse_args(argc, argv);

        /* ================= DIGEST MODE ================= */

        if (args.command == "dgst") {
            if (args.hmac) {
                auto key = hex_to_bytes(args.key_hex);
                std::ifstream in(args.input_file, std::ios::binary);
                if (!in) return 1;

                auto tag = hmac_sha256(key, in);

                std::ostringstream oss;
                oss << std::hex << std::setfill('0');
                for (auto b : tag)
                    oss << std::setw(2) << (int)b;
                std::string hex = oss.str();

                if (!args.verify_file.empty()) {
                    std::ifstream vf(args.verify_file);
                    std::string expected;
                    vf >> expected;

                    if (expected == hex) {
                        std::cout << "[OK] HMAC verification successful\n";
                        return 0;
                    }

                    std::cerr << "[ERROR] HMAC verification failed\n";
                    return 1;
                }

                if (!args.output_file.empty()) {
                    std::ofstream out(args.output_file, std::ios::binary);
                    if (!out) {
                        std::cerr << "[ERROR] Cannot write output file\n";
                        return 1;
                    }
                    out << hex;
                    std::cout << "[OK] HMAC computed successfully\n";
                }
                else {
                    std::cout << hex << "\n";
                }
                return 0;

            }
        }

        /* ================= CIPHER MODE ================= */

        bool needs_iv = (args.mode != "ecb" && args.mode != "gcm" && args.mode != "etm");
        auto key = hex_to_bytes(args.key_hex);

        /* ================= ENCRYPT ================= */

        if (args.encrypt) {
            auto plaintext = read_file(args.input_file);

            /* ===== GCM ===== */
            if (args.mode == "gcm") {
                std::vector<uint8_t> aad;
                if (!args.aad_hex.empty())
                    aad = hex_to_bytes(args.aad_hex);

                auto nonce = generate_random_bytes(12);
                auto res = modes::gcm_encrypt(key, plaintext, aad, nonce);

                std::vector<uint8_t> out;
                out.insert(out.end(), res.nonce.begin(), res.nonce.end());
                out.insert(out.end(), res.ciphertext.begin(), res.ciphertext.end());
                out.insert(out.end(), res.tag.begin(), res.tag.end());

                write_file(args.output_file, out);
                std::cout << "[OK] AES-GCM encryption successful\n";
                return 0;
            }

            /* ===== Encrypt-then-MAC ===== */
            if (args.mode == "etm") {
                std::vector<uint8_t> aad;
                if (!args.aad_hex.empty())
                    aad = hex_to_bytes(args.aad_hex);

                std::vector<uint8_t> k_enc = key;
                std::vector<uint8_t> k_mac = key;
                k_mac.push_back(0x01);

                auto iv = generate_random_bytes(16);
                auto ciphertext = modes::encrypt_ctr(k_enc, plaintext, iv);

                std::vector<uint8_t> mac_input = ciphertext;
                mac_input.insert(mac_input.end(), aad.begin(), aad.end());

                std::string mac_buf(
                    reinterpret_cast<char*>(mac_input.data()),
                    mac_input.size()
                );
                std::istringstream mac_stream(mac_buf);
                auto tag = hmac_sha256(k_mac, mac_stream);

                std::vector<uint8_t> out;
                out.insert(out.end(), iv.begin(), iv.end());
                out.insert(out.end(), ciphertext.begin(), ciphertext.end());
                out.insert(out.end(), tag.begin(), tag.end());

                write_file(args.output_file, out);
                std::cout << "[OK] Encrypt-then-MAC encryption successful\n";
                return 0;
            }

            /* ===== OLD MODES ===== */
            std::vector<uint8_t> iv(16);
            if (needs_iv)
                iv = generate_random_bytes(16);

            std::vector<uint8_t> cipher;
            if (args.mode == "ecb")      cipher = ecb_encrypt(key, plaintext);
            else if (args.mode == "cbc") cipher = modes::encrypt_cbc(key, plaintext, iv);
            else if (args.mode == "cfb") cipher = modes::encrypt_cfb(key, plaintext, iv);
            else if (args.mode == "ofb") cipher = modes::encrypt_ofb(key, plaintext, iv);
            else if (args.mode == "ctr") cipher = modes::encrypt_ctr(key, plaintext, iv);

            if (needs_iv)
                write_file_with_iv(args.output_file, iv, cipher);
            else
                write_file(args.output_file, cipher);

            std::cout << "[OK] AES-" << args.mode << " encryption successful\n";
            return 0;
        }

        /* ================= DECRYPT ================= */

        /* ===== GCM ===== */
        if (args.mode == "gcm") {
            auto input = read_file(args.input_file);
            if (input.size() < 12 + 16)
                return 1;

            std::vector<uint8_t> aad;
            if (!args.aad_hex.empty())
                aad = hex_to_bytes(args.aad_hex);

            std::vector<uint8_t> nonce;
            if (!args.iv_hex.empty()) {
                nonce = hex_to_bytes(args.iv_hex);
                if (nonce.size() != 12) {
                    std::cerr << "[ERROR] GCM nonce must be 12 bytes\n";
                    return 1;
                }
            }
            else {
                nonce.assign(input.begin(), input.begin() + 12);
            }

            std::vector<uint8_t> tag(input.end() - 16, input.end());
            std::vector<uint8_t> cipher(input.begin() + 12, input.end() - 16);

            std::vector<uint8_t> plaintext;
            if (!modes::gcm_decrypt(key, nonce, cipher, aad, tag, plaintext)) {
                std::cerr << "[ERROR] Authentication failed (GCM)\n";
                return 1;
            }

            write_file(args.output_file, plaintext);
            std::cout << "[OK] AES-GCM authentication successful\n";
            return 0;
        }

        /* ===== Encrypt-then-MAC ===== */
        if (args.mode == "etm") {
            auto input = read_file(args.input_file);
            if (input.size() < 16 + 32)
                return 1;

            std::vector<uint8_t> aad;
            if (!args.aad_hex.empty())
                aad = hex_to_bytes(args.aad_hex);

            std::vector<uint8_t> k_enc = key;
            std::vector<uint8_t> k_mac = key;
            k_mac.push_back(0x01);

            std::vector<uint8_t> iv(input.begin(), input.begin() + 16);
            std::vector<uint8_t> tag(input.end() - 32, input.end());
            std::vector<uint8_t> cipher(input.begin() + 16, input.end() - 32);

            std::vector<uint8_t> mac_input = cipher;
            mac_input.insert(mac_input.end(), aad.begin(), aad.end());

            std::string mac_buf(
                reinterpret_cast<char*>(mac_input.data()),
                mac_input.size()
            );
            std::istringstream mac_stream(mac_buf);
            auto expected = hmac_sha256(k_mac, mac_stream);

            uint8_t diff = 0;
            for (size_t i = 0; i < tag.size(); ++i)
                diff |= tag[i] ^ expected[i];

            if (diff != 0) {
                std::cerr << "[ERROR] Authentication failed (ETM)\n";
                return 1;
            }

            auto plaintext = modes::decrypt_ctr(k_enc, cipher, iv);
            write_file(args.output_file, plaintext);
            std::cout << "[OK] Encrypt-then-MAC authentication successful\n";
            return 0;
        }

        /* ===== OLD MODES DECRYPT ===== */

        std::vector<uint8_t> iv;
        std::vector<uint8_t> cipher;

        if (needs_iv)
            cipher = read_file_with_iv(args.input_file, iv);
        else
            cipher = read_file(args.input_file);

        std::vector<uint8_t> plaintext;

        if (args.mode == "ecb")      plaintext = ecb_decrypt(key, cipher);
        else if (args.mode == "cbc") plaintext = modes::decrypt_cbc(key, cipher, iv);
        else if (args.mode == "cfb") plaintext = modes::decrypt_cfb(key, cipher, iv);
        else if (args.mode == "ofb") plaintext = modes::decrypt_ofb(key, cipher, iv);
        else if (args.mode == "ctr") plaintext = modes::decrypt_ctr(key, cipher, iv);
        else {
            std::cerr << "[ERROR] Unknown mode\n";
            return 1;
        }

        write_file(args.output_file, plaintext);
        std::cout << "[OK] AES-" << args.mode << " decryption successful\n";
        return 0;
    }
    catch (...) {
        std::cerr << "[ERROR] Fatal error\n";
        return 1;
    }
}

