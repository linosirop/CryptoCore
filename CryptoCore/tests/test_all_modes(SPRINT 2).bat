@echo off
setlocal enabledelayedexpansion

echo ===========================================
echo         AES TORTURE TEST START
echo ===========================================

REM --- Paths ---
set TEST_DIR=%~dp0
set EXE="C:\Users\User\source\repos\CryptoCore\x64\Debug\CryptoCore.exe"
set OPENSSL="C:\Program Files\OpenSSL-Win64\bin\openssl.exe"

echo [INFO] Using CryptoCore at %EXE%
echo [INFO] Using OpenSSL at %OPENSSL%
echo.

REM --- Test parameters ---
set KEY=00112233445566778899aabbccddeeff
set IV=AABBCCDDEEFF00112233445566778899

REM --- File names in tests folder ---
set PLAINTEXT=%TEST_DIR%plain.bin
set SSL_OUT=%TEST_DIR%ssl.bin
set CORE_OUT=%TEST_DIR%core.bin
set DECRYPT_OUT=%TEST_DIR%decrypt.bin

REM --- Sizes to test ---
set SIZES=16 31 32 47 48 63 64 100 256 1024 4096 16384 65536 131072 262144 524288 1048576

for %%S in (%SIZES%) do (
    echo -------------------------------------------
    echo Testing size %%S bytes
    echo -------------------------------------------

    REM Generate plaintext
    powershell -Command "$d=%%S; $b=New-Object byte[]($d); (New-Object Random).NextBytes($b); [IO.File]::WriteAllBytes('%PLAINTEXT%', $b)"

    REM Test each AES mode
    for %%M in (ecb cbc cfb ofb ctr) do (

        echo   -- Testing mode: %%M

        REM === Encrypt with OpenSSL ===
        if "%%M"=="ecb" (
            %OPENSSL% enc -aes-128-%%M -K %KEY% -in "%PLAINTEXT%" -out "%SSL_OUT%"
        ) else (
            %OPENSSL% enc -aes-128-%%M -K %KEY% -iv %IV% -in "%PLAINTEXT%" -out "%SSL_OUT%"
        )

        REM === Encrypt with CryptoCore ===
        if "%%M"=="ecb" (
            %EXE% --algorithm aes --mode %%M --encrypt --key %KEY% --input "%PLAINTEXT%" --output "%CORE_OUT%"
        ) else (
            %EXE% --algorithm aes --mode %%M --encrypt --key %KEY% --iv %IV% --input "%PLAINTEXT%" --output "%CORE_OUT%"
        )

        REM Compare ciphertexts
        fc /b "%SSL_OUT%" "%CORE_OUT%" >nul
        if errorlevel 1 (
            echo     [FAIL] Ciphertext mismatch in %%M for size %%S
            exit /b 1
        ) else (
            echo     [OK] Encryption match
        )

        REM === Decrypt back ===
        if "%%M"=="ecb" (
            %EXE% --algorithm aes --mode %%M --decrypt --key %KEY% --input "%SSL_OUT%" --output "%DECRYPT_OUT%"
        ) else (
            %EXE% --algorithm aes --mode %%M --decrypt --key %KEY% --iv %IV% --input "%SSL_OUT%" --output "%DECRYPT_OUT%"
        )

        REM Compare decrypted plaintext with original
        fc /b "%PLAINTEXT%" "%DECRYPT_OUT%" >nul
        if errorlevel 1 (
            echo     [FAIL] Decryption mismatch in %%M for size %%S
            exit /b 1
        ) else (
            echo     [OK] Decryption match
        )
    )
)

echo.
echo ===========================================
echo       ALL TORTURE TESTS PASSED
echo ===========================================

endlocal
pause
