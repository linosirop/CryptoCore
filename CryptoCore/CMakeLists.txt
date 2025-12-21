cmake_minimum_required(VERSION 3.10)
project(CryptoCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

# =========================
# Main executable
# =========================
add_executable(cryptocore
    CryptoCore.cpp
    src/aes_primitive.cpp
    src/cli_parser.cpp
    src/csprng.cpp
    src/file_io.cpp
    src/padding.cpp

    src/aead/etm.cpp

    src/hash/sha3_stream.cpp
    src/hash/sha256_stream.cpp
    src/mac/hmac.cpp

    src/modes/cbc.cpp
    src/modes/cfb.cpp
    src/modes/ctr.cpp
    src/modes/ecb.cpp
    src/modes/ofb.cpp
    src/modes/gcm.cpp

    src/kdf/pbkdf2.cpp
    src/kdf/hkdf.cpp
)

target_include_directories(cryptocore PRIVATE
    include
    include/modes
    src
    src/aead
    src/hash
    src/mac
    src/modes
    src/kdf
)

target_link_libraries(cryptocore PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

# =========================
# Tests
# =========================
enable_testing()

add_executable(test_pbkdf2
    tests/test_pbkdf2.cpp
    src/kdf/pbkdf2.cpp
    src/mac/hmac.cpp
    src/hash/sha256_stream.cpp
)

target_include_directories(test_pbkdf2 PRIVATE
    include
    src
    src/hash
    src/mac
    src/kdf
)

target_link_libraries(test_pbkdf2 PRIVATE
    OpenSSL::Crypto
)

add_executable(test_hkdf
    tests/test_hkdf.cpp
    src/kdf/hkdf.cpp
    src/mac/hmac.cpp
    src/hash/sha256_stream.cpp
)

target_include_directories(test_hkdf PRIVATE
    include
    src
    src/hash
    src/mac
    src/kdf
)

target_link_libraries(test_hkdf PRIVATE
    OpenSSL::Crypto
)

add_test(NAME pbkdf2 COMMAND test_pbkdf2)
add_test(NAME hkdf COMMAND test_hkdf)
