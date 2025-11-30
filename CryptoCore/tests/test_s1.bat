@echo off
echo Hello ECB! > plaintext.txt

cryptocore.exe --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input plaintext.txt --output ciphertext.bin

cryptocore.exe --algorithm aes --mode ecb --decrypt --key 000102030405060708090a0b0c0d0e0f --input ciphertext.bin --output decrypted.txt

fc plaintext.txt decrypted.txt >nul
if %errorlevel% == 0 (
    echo [SUCCESS] Round-trip OK!
) else (
    echo [FAIL] Round-trip failed!
)

del plaintext.txt ciphertext.bin decrypted.txt
pause