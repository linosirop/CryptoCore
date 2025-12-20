cmake_minimum_required(VERSION 3.10)
project(CryptoCore)

# Установите стандарт C++ (если ваш код требует)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найдите OpenSSL (через vcpkg)
find_package(OpenSSL REQUIRED)

# Список всех исходных файлов (.cpp)
add_executable(cryptocore
    CryptoCore.cpp                  # Основной файл в корне (если нет — удалите)
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
    # Если в src/modes/gcm/ есть .cpp — добавьте, напр. src/modes/gcm/some.cpp
    # Если есть другие .cpp (в tests/ или др.) — добавьте или сделайте отдельную цель
)

# Директории для заголовков (.h) — все пути, где лежат .h
target_include_directories(cryptocore PRIVATE
    include
    include/modes
    src
    src/aead
    src/hash
    src/mac
    src/modes
    
)

# Свяжите библиотеки (OpenSSL)
target_link_libraries(cryptocore PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

