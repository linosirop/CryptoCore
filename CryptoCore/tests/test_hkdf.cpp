#include <cassert>
#include <iostream>
#include <vector>
#include <string>

#include "hkdf.h"

static std::vector<uint8_t> bytes_from_char(char c, size_t n) {
    return std::vector<uint8_t>(n, static_cast<uint8_t>(c));
}

static void print_hex(const std::vector<uint8_t>& v) {
    static const char* hexd = "0123456789abcdef";
    for (uint8_t b : v) {
        std::cout << hexd[b >> 4] << hexd[b & 0x0F];
    }
    std::cout << "\n";
}

int main() {
    std::vector<uint8_t> master = bytes_from_char('0', 32);

    // TEST-5: Deterministic output
    auto k1a = derive_key(master, "encryption", 32);
    auto k1b = derive_key(master, "encryption", 32);
    assert(k1a == k1b);
    std::cout << "[OK] Deterministic test passed\n";

    // TEST-6: Context separation
    auto k2 = derive_key(master, "authentication", 32);
    assert(k1a != k2);
    std::cout << "[OK] Context separation test passed\n";

    // Extra: different length
    auto k3 = derive_key(master, "encryption", 64);
    assert(k3.size() == 64);
    std::cout << "[OK] Length test passed (64 bytes)\n";

    std::cout << "Key(encryption,32): ";
    print_hex(k1a);
    std::cout << "Key(authentication,32): ";
    print_hex(k2);

    std::cout << "[OK] All HKDF-like tests passed\n";
    return 0;
}
