cmake_minimum_required(VERSION 3.20)
project(CryptoCore VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск OpenSSL
find_package(OpenSSL REQUIRED)
message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")

# Включение директорий с заголовками
include_directories(${CMAKE_SOURCE_DIR}/include)

# Создание исполняемого файла с абсолютными путями
add_executable(cryptocore
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/cli_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/file_io.cpp
    ${CMAKE_SOURCE_DIR}/src/modes/ecb.cpp
)

# Связывание с OpenSSL
target_link_libraries(cryptocore OpenSSL::Crypto)

# Для Windows: настройки компилятора
if(MSVC)
    target_compile_options(cryptocore PRIVATE /W4)
endif()