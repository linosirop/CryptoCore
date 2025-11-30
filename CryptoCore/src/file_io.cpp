
#include "file_io.h"
#include <fstream>
#include <stdexcept>

std::vector<uint8_t> read_file(const std::string& filename) {  // CRY-5
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Cannot open input: " + filename);

    auto size = file.tellg();
    file.seekg(0);
    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Read error: " + filename);
    }
    return buffer;
}

void write_file(const std::string& filename, const std::vector<uint8_t>& data) {  // CRY-5
    std::ofstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open output: " + filename);

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!file) throw std::runtime_error("Write error: " + filename);
}

std::vector<uint8_t> read_file_with_iv(const std::string& filename, std::vector<uint8_t>& iv_out) {
    auto data = read_file(filename);
    if (data.size() < 16) throw std::runtime_error("File too short for IV");
    iv_out.assign(data.begin(), data.begin() + 16);
    return {data.begin() + 16, data.end()};
}

void write_file_with_iv(const std::string& filename, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> full = iv;
    full.insert(full.end(), data.begin(), data.end());
    write_file(filename, full);
}

