@echo off
echo Testing Sprint 2 modes...

echo Hello CryptoCore Sprint 2! > test_input.txt

echo Testing CBC mode...
cryptocore.exe --algorithm aes --mode cbc --encrypt --key 000102030405060708090a0b0c0d0e0f --input test_input.txt --output test_cbc_enc.bin
cryptocore.exe --algorithm aes --mode cbc --decrypt --key 000102030405060708090a0b0c0d0e0f --input test_cbc_enc.bin --output test_cbc_dec.txt
fc test_input.txt test_cbc_dec.txt >nul
if %errorlevel% == 0 (echo [SUCCESS] CBC round-trip OK!) else (echo [FAIL] CBC round-trip failed!)

echo Testing CTR mode...
cryptocore.exe --algorithm aes --mode ctr --encrypt --key 000102030405060708090a0b0c0d0e0f --input test_input.txt --output test_ctr_enc.bin
cryptocore.exe --algorithm aes --mode ctr --decrypt --key 000102030405060708090a0b0c0d0e0f --input test_ctr_enc.bin --output test_ctr_dec.txt
fc test_input.txt test_ctr_dec.txt >nul
if %errorlevel% == 0 (echo [SUCCESS] CTR round-trip OK!) else (echo [FAIL] CTR round-trip failed!)

del test_input.txt test_*_enc.bin test_*_dec.txt
echo Test completed!
pause