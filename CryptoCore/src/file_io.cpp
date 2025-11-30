
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

