#pragma once
#include <vector>
#include <string>
#include <cstdint>

std::vector<uint8_t> read_file(const std::string& filename);
void write_file(const std::string& filename, const std::vector<uint8_t>& data);

std::vector<uint8_t> read_file_with_iv(
    const std::string& filename,
    std::vector<uint8_t>& iv_out
);

void write_file_with_iv(
    const std::string& filename,
    const std::vector<uint8_t>& iv,
    const std::vector<uint8_t>& data
);
//cryptocore --algorithm aes --mode etm --encrypt --key 00112233445566778899aabbccddeeff --input plain.txt --output etm.bin --aad aabbcc

//cryptocore --algorithm aes --mode etm --decrypt --key 00112233445566778899aabbccddeeff --input etm.bin --output dec.txt --aad aabbcc
