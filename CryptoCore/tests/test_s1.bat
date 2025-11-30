@echo off
echo [TEST] Starting automatic round-trip test...
echo.

:: Поиск cryptocore.exe во всех возможных расположениях
set EXE_PATH=..\..\x64\Debug\CryptoCore.exe
if exist "%EXE_PATH%" goto found

set EXE_PATH=..\..\x64\Release\CryptoCore.exe
if exist "%EXE_PATH%" goto found

set EXE_PATH=..\..\build\Release\cryptocore.exe
if exist "%EXE_PATH%" goto found

set EXE_PATH=..\..\build\Debug\cryptocore.exe
if exist "%EXE_PATH%" goto found

set EXE_PATH=..\x64\Debug\CryptoCore.exe
if exist "%EXE_PATH%" goto found

set EXE_PATH=..\x64\Release\CryptoCore.exe
if exist "%EXE_PATH%" goto found

set EXE_PATH=CryptoCore.exe
if exist "%EXE_PATH%" goto found

echo [ERROR] CryptoCore.exe not found!
echo.
echo Please ensure the project is built and the executable exists in one of these locations:
echo   ..\..\x64\Debug\CryptoCore.exe
echo   ..\..\x64\Release\CryptoCore.exe
echo   ..\..\build\Release\cryptocore.exe
echo   ..\..\build\Debug\cryptocore.exe
echo   ..\x64\Debug\CryptoCore.exe
echo   ..\x64\Release\CryptoCore.exe
echo.
pause
exit /b 1

:found
echo [INFO] Using: %EXE_PATH%
echo.

:: Создание тестового файла
echo Hello CryptoCore! Automated test. > plaintext.txt

:: Шифрование
echo [TEST] Encrypting plaintext.txt...
"%EXE_PATH%" --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input plaintext.txt --output ciphertext.bin
if errorlevel 1 (
    echo [ERROR] Encryption failed!
    goto cleanup
)

:: Дешифрование
echo [TEST] Decrypting ciphertext.bin...
"%EXE_PATH%" --algorithm aes --mode ecb --decrypt --key 000102030405060708090a0b0c0d0e0f --input ciphertext.bin --output decrypted.txt
if errorlevel 1 (
    echo [ERROR] Decryption failed!
    goto cleanup
)

:: Сравнение файлов
echo [TEST] Comparing files...
fc plaintext.txt decrypted.txt >nul
if %errorlevel% == 0 (
    echo [SUCCESS] Round-trip test PASSED! Files are identical.
) else (
    echo [FAIL] Round-trip test FAILED! Files are different.
)

:cleanup
echo.
echo [TEST] Cleaning up...
del plaintext.txt ciphertext.bin decrypted.txt 2>nul
echo [TEST] Test completed.
pause