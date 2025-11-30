
#pragma once
#include <vector>
#include <string>
#include <cstdint>

std::vector<uint8_t> read_file(const std::string& filename);  // CRY-5: binary
void write_file(const std::string& filename, const std::vector<uint8_t>& data);

std::vector<uint8_t> read_file_with_iv(const std::string& filename, std::vector<uint8_t>& iv_out);
void write_file_with_iv(const std::string& filename, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& data);
