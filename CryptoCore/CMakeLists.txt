cmake_minimum_required(VERSION 3.20)
project(CryptoCore VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")

include_directories(include)

add_executable(cryptocore
    CryptoCore.cpp
    src/cli_parser.cpp
    src/file_io.cpp
    src/modes/ecb.cpp
)

target_link_libraries(cryptocore OpenSSL::Crypto)

if(MSVC)
    target_compile_options(cryptocore PRIVATE /W4)
endif()