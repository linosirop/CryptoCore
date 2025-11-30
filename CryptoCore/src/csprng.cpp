#include "csprng.h"
#include <openssl/rand.h>
#include <stdexcept>

std::vector<uint8_t> generate_random_bytes(size_t num_bytes) {
    std::vector<uint8_t> bytes(num_bytes);
    if (RAND_bytes(bytes.data(), static_cast<int>(num_bytes)) != 1) {
        throw std::runtime_error("CSPRNG failed: RAND_bytes error");
    }
    return bytes;
}