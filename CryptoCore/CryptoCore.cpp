#include "cli_parser.h"
#include "file_io.h"
#include "csprng.h"
#include "padding.h"

#include "modes/ecb.h"
#include "modes/cbc.h"
#include "modes/cfb.h"
#include "modes/ofb.h"
#include "modes/ctr.h"

#include "C:\Users\User\source\repos\CryptoCore\CryptoCore\src\hash\sha256_stream.h"
#include "C:\Users\User\source\repos\CryptoCore\CryptoCore\src\hash\sha3_stream.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include<algorithm>
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
            std::cerr << "[ERROR] Invalid hex character in: " << hex << "\n";
            exit(1);
        }

        out.push_back((uint8_t)((hi << 4) | lo));
    }

    return out;
}


static void print_hex(const std::vector<uint8_t>& buf) {
    std::cout << std::hex << std::setfill('0');
    for (uint8_t b : buf)
        std::cout << std::setw(2) << (int)b;
    std::cout << std::dec << "\n";
}

int main(int argc, char* argv[]) {
    try {
        CliArgs args = parse_args(argc, argv);

        
      // DIGEST MODE (SHA-256, SHA3-256)
   
        if (args.command == "dgst") {

            const size_t CHUNK = 64 * 1024; 

            std::ifstream in(args.input_file, std::ios::binary);
            if (!in) {
                std::cerr << "[ERROR] Unable to open input file: " << args.input_file << std::endl;
                return 1;
            }

            std::string alg = args.algorithm;
            std::transform(alg.begin(), alg.end(), alg.begin(), ::tolower);

            std::vector<uint8_t> digest;

            
            // SHA-256 STREAMING
            
            if (alg == "sha256") {

                SHA256_CTX ctx;
                sha256_init(ctx);

                std::vector<uint8_t> buf(CHUNK);

                while (true) {
                    in.read(reinterpret_cast<char*>(buf.data()), buf.size());
                    std::streamsize r = in.gcount();
                    if (r <= 0) break;
                    sha256_update(ctx, buf.data(), static_cast<size_t>(r));
                }

                digest = sha256_final(ctx);
            }

            
            // SHA3-256 STREAMING
           
            else if (alg == "sha3-256") {

                SHA3_CTX ctx3;
                sha3_256_init(ctx3);

                std::vector<uint8_t> buf(CHUNK);

                while (true) {
                    in.read(reinterpret_cast<char*>(buf.data()), buf.size());
                    std::streamsize r = in.gcount();
                    if (r <= 0) break;
                    sha3_256_update(ctx3, buf.data(), static_cast<size_t>(r));
                }

                digest = sha3_256_final(ctx3);
            }

            else {
                std::cerr << "[ERROR] Unsupported algorithm: " << args.algorithm << std::endl;
                return 1;
            }

            in.close();

            
            // Convert digest to lowercase HEX
            
            std::ostringstream oss;
            oss << std::hex << std::setfill('0');
            for (uint8_t b : digest)
                oss << std::setw(2) << (unsigned int)(b & 0xFF);

            std::string hex = oss.str();

            
            
            
            std::cout << hex << " " << args.input_file << std::endl;
            std::cout << "[INFO] Digest completed." << std::endl;

            
            
            
            if (!args.output_file.empty()) {
                std::ofstream out(args.output_file, std::ios::binary);
                if (!out) {
                    std::cerr << "[ERROR] Unable to open output file: " << args.output_file << std::endl;
                    return 1;
                }

                out << hex << " " << args.input_file;
                out.close();
            }

            return 0;
        }





        
        // CIPHER MODE (AES)
        
        bool needs_iv = (args.mode != "ecb");

        // random dump
        if (args.dump_random > 0) {
            auto rnd = generate_random_bytes(args.dump_random);
            write_file(args.output_file, rnd);
            std::cout << "[INFO] Wrote random bytes.\n";
            return 0;
        }

        if (args.encrypt) {
            auto plaintext = read_file(args.input_file);

            std::vector<uint8_t> key;
            if (args.key_hex.empty()) {
                key = generate_random_bytes(16);
            }
            else {
                key = hex_to_bytes(args.key_hex);
                bool zero = true, seq = true;
                for (int i = 0; i < 16; i++) {
                    if (key[i] != 0) zero = false;
                    if (i > 0 && key[i] != key[i - 1] + 1) seq = false;
                }
                if (zero)  std::cerr << "[WEAK-KEY-ZERO]\n";
                if (seq)   std::cerr << "[WEAK-KEY-SEQ]\n";
            }

            std::vector<uint8_t> iv(16);
            if (needs_iv) {
                if (!args.iv_hex.empty())
                    iv = hex_to_bytes(args.iv_hex);
                else
                    iv = generate_random_bytes(16);
            }

            std::vector<uint8_t> cipher;
            if (args.mode == "ecb") cipher = ecb_encrypt(key, plaintext);
            else if (args.mode == "cbc") cipher = modes::encrypt_cbc(key, plaintext, iv);
            else if (args.mode == "cfb") cipher = modes::encrypt_cfb(key, plaintext, iv);
            else if (args.mode == "ofb") cipher = modes::encrypt_ofb(key, plaintext, iv);
            else if (args.mode == "ctr") cipher = modes::encrypt_ctr(key, plaintext, iv);

            if (needs_iv && args.iv_hex.empty())
                write_file_with_iv(args.output_file, iv, cipher);
            else
                write_file(args.output_file, cipher);

            std::cout << "[INFO] Encryption completed.\n";
            return 0;
        }

        // decrypt
        auto key = hex_to_bytes(args.key_hex);
        std::vector<uint8_t> iv(16);
        std::vector<uint8_t> enc;

        if (needs_iv) {
            if (args.iv_hex.empty())
                enc = read_file_with_iv(args.input_file, iv);
            else {
                iv = hex_to_bytes(args.iv_hex);
                enc = read_file(args.input_file);
            }
        }
        else {
            enc = read_file(args.input_file);
        }

        std::vector<uint8_t> out;
        if (args.mode == "ecb") out = ecb_decrypt(key, enc);
        else if (args.mode == "cbc") out = modes::decrypt_cbc(key, enc, iv);
        else if (args.mode == "cfb") out = modes::decrypt_cfb(key, enc, iv);
        else if (args.mode == "ofb") out = modes::decrypt_ofb(key, enc, iv);
        else if (args.mode == "ctr") out = modes::decrypt_ctr(key, enc, iv);

        write_file(args.output_file, out);
        std::cout << "[INFO] Decryption completed.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }
}
