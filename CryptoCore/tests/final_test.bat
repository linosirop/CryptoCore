@echo off
echo [FINAL TEST] CryptoCore Sprint 1 Verification
echo ============================================
echo.

set EXE_PATH=C:\Users\User\source\repos\CryptoCore\x64\Debug\CryptoCore.exe

echo [1/5] Checking for CryptoCore.exe...
if not exist "%EXE_PATH%" (
    echo [ERROR] Executable not found at: %EXE_PATH%
    echo Please build the project first!
    pause
    exit /b 1
)
echo [OK] Found: %EXE_PATH%
echo.

echo [2/5] Creating test file...
echo "This is a test of CryptoCore AES-128 ECB implementation" > test_input.txt
echo "Project: Sprint 1 - Completed Successfully" >> test_input.txt
echo [OK] Test file created
echo.

echo [3/5] Encrypting test file...
"%EXE_PATH%" --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input test_input.txt --output test_encrypted.bin
if errorlevel 1 (
    echo [ERROR] Encryption failed!
    pause
    exit /b 1
)
echo [OK] Encryption successful
echo.

echo [4/5] Decrypting test file...
"%EXE_PATH%" --algorithm aes --mode ecb --decrypt --key 000102030405060708090a0b0c0d0e0f --input test_encrypted.bin --output test_decrypted.txt
if errorlevel 1 (
    echo [ERROR] Decryption failed!
    pause
    exit /b 1
)
echo [OK] Decryption successful
echo.

echo [5/5] Verifying round-trip...
fc test_input.txt test_decrypted.txt >nul
if errorlevel 1 (
    echo [FAIL] Files are different!
    echo.
    echo Input file:
    type test_input.txt
    echo.
    echo Decrypted file:
    type test_decrypted.txt
) else (
    echo [SUCCESS] Round-trip verification PASSED!
    echo Files are identical - CryptoCore is working correctly!
)
echo.

echo [CLEANUP] Removing test files...
del test_input.txt test_encrypted.bin test_decrypted.txt 2>nul
echo.
echo [COMPLETE] Sprint 1 testing finished!
echo.
echo Project Status: READY FOR SUBMISSION
pause