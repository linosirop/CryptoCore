#include <cassert>
#include <iostream>
#include <vector>
#include <string>

#include "pbkdf2.h"

static std::vector<uint8_t> str(const std::string& s) {
    return std::vector<uint8_t>(s.begin(), s.end());
}

static std::vector<uint8_t> hex(const std::string& s) {
    std::vector<uint8_t> out;
    for (size_t i = 0; i < s.size(); i += 2)
        out.push_back(static_cast<uint8_t>(std::stoul(s.substr(i, 2), nullptr, 16)));
    return out;
}

void test_deterministic() {
    auto dk1 = pbkdf2_hmac_sha256(
        str("password"),
        str("salt"),
        1000,
        32
    );

    auto dk2 = pbkdf2_hmac_sha256(
        str("password"),
        str("salt"),
        1000,
        32
    );

    assert(dk1 == dk2);
    std::cout << "[OK] Deterministic test passed\n";
}

void test_lengths() {
    for (size_t len = 1; len <= 100; ++len) {
        auto dk = pbkdf2_hmac_sha256(
            str("password"),
            str("salt"),
            1000,
            len
        );
        assert(dk.size() == len);
    }
    std::cout << "[OK] Length test passed (1..100 bytes)\n";
}

void test_openssl_vector() {
    /*
     * Generated with:
     * openssl kdf -keylen 32 \
     *   -kdfopt pass:password \
     *   -kdfopt salt:73616c74 \
     *   -kdfopt iter:1 PBKDF2
     */

    auto dk = pbkdf2_hmac_sha256(
        str("password"),
        hex("73616c74"),
        1,
        32
    );

    auto expected = hex(
        "120fb6cffcf8b32c43e7225256c4f837"
        "a86548c92ccc35480805987cb70be17b"
    );

    assert(dk == expected);
    std::cout << "[OK] OpenSSL compatibility test passed\n";
}

int main() {
    test_deterministic();
    test_lengths();
    test_openssl_vector();

    std::cout << "[OK] All PBKDF2-SHA256 tests passed\n";
    return 0;
}
