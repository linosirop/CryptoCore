# CryptoCore — User Guide

## 1. Requirements

### Windows
- Visual Studio 2022 or MinGW (g++)
- CMake >= 3.10
- OpenSSL (development package)

### Linux (Fedora / Arch / Ubuntu)
- CMake >= 3.10
- g++ (or clang++)
- OpenSSL development headers

Fedora:
sudo dnf install -y cmake gcc-c++ openssl-devel

Arch:
sudo pacman -S --needed cmake gcc openssl

Ubuntu/Debian:
sudo apt update
sudo apt install -y cmake g++ libssl-dev

---

## 2. Build (Linux)

From project root:

git clone https://github.com/linosirop/CryptoCore
cd <ПАПКА_ПРОЕКТА>

cmake -S . -B build
cmake --build build -j

The executable will be:
build/cryptocore

Run it like:
./build/cryptocore --help

---

## 3. Build (Windows, CMake)

From project root:

cmake -S . -B build
cmake --build build --config Debug -j

Executable path depends on generator.
For Visual Studio it is usually:
build/Debug/cryptocore.exe
or build/cryptocore.exe

---

## 4. AES Encryption / Decryption

### Key sizes
This project expects AES-128 keys by default:
- AES-128 key: 16 bytes = 32 hex characters

Examples below use AES-128.

### ECB mode (no IV)

Encrypt:
cryptocore --algorithm aes --mode ecb --encrypt --key 00112233445566778899aabbccddeeff --input plain.txt --output ecb.bin

Decrypt:
cryptocore --algorithm aes --mode ecb --decrypt --key 00112233445566778899aabbccddeeff --input ecb.bin --output ecb.txt

---

### CBC / CFB / OFB / CTR modes

These modes use an IV (16 bytes = 32 hex characters).
If IV is not provided, the tool may generate it automatically depending on implementation.

CBC with explicit IV:

Encrypt:
cryptocore --algorithm aes --mode cbc --encrypt --key 00112233445566778899aabbccddeeff --iv 0102030405060708090a0b0c0d0e0f10 --input plain.txt --output cbc.bin

Decrypt:
cryptocore --algorithm aes --mode cbc --decrypt --key 00112233445566778899aabbccddeeff --iv 0102030405060708090a0b0c0d0e0f10 --input cbc.bin --output out.txt

---

## 5. AEAD (GCM / ETM)

### GCM
GCM uses a nonce (typically 12 bytes). Some implementations accept it via --iv.

Example (if supported by your build):
cryptocore --algorithm aes --mode gcm --encrypt --key 00112233445566778899aabbccddeeff --input plain.txt --output gcm.bin

---

## 6. Key Derivation (Sprint 7)

### PBKDF2 derive command

Basic (explicit salt):
cryptocore derive --password "MySecurePassword123!" --salt a1b2c3d4e5f601234567890123456789 --iterations 100000 --length 32 --algorithm pbkdf2

Output format:
KEY_HEX SALT_HEX

Auto-generated salt (if --salt is omitted):
cryptocore derive --password "AnotherPassword" --iterations 500000 --length 16 --algorithm pbkdf2

Write derived key to file as raw bytes (if supported):
cryptocore derive --password "app_key" --salt 73616c74 --iterations 100000 --length 32 --algorithm pbkdf2 --output key.bin

Security notes:
- Use a random salt (at least 16 bytes).
- Use high iteration count (e.g. 100,000+; more for stronger security).
- Avoid passing passwords in shell history when possible.

---

## 7. Tests

### Build and run CTest (Linux)
cmake -S . -B build
cmake --build build -j
cd build
ctest --output-on-failure

### Regression examples (old modes)
Create input:
echo HelloOldModes > plain.txt

ECB round-trip:
cryptocore --algorithm aes --mode ecb --encrypt --key 00112233445566778899aabbccddeeff --input plain.txt --output ecb.bin
cryptocore --algorithm aes --mode ecb --decrypt --key 00112233445566778899aabbccddeeff --input ecb.bin --output ecb.txt

---

## 8. Troubleshooting

### "[ERROR] Fatal error"
Most common reasons:
- Key hex length is not supported (e.g. passing 32-byte key where only 16-byte AES-128 is implemented).
- IV/nonce length is wrong for the selected mode.
- Input/output path errors or file permission issues.

Check:
- AES-128 key must be exactly 32 hex characters.
- CBC/CFB/OFB/CTR IV must be exactly 32 hex characters.