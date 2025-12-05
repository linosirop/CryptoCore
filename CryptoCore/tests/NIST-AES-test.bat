@echo off
setlocal enabledelayedexpansion

rem ---------------------------
rem CONFIG — поправь пути если нужно
rem ---------------------------
set "CRYPTOCORE=C:\Users\User\source\repos\CryptoCore\x64\Debug\CryptoCore.exe"
set "OPENSSL=C:\Program Files\OpenSSL-Win64\bin\openssl.exe"

rem Если хочешь видеть предупреждения о слабом ключе — выставь 1, иначе 0
set "SHOW_WEAK=0"

rem Тестовый ключ (16 байт hex) — поменяй при нужде
set "KEY=A1B2C3D4E5F60718293A4B5C6D7E8F90"

rem список размеров (пример)
set "SIZES=16 31 32 47 48 63 64 100 256 1024"

rem режимы
set "MODES=ecb cbc cfb ofb ctr"

echo ===========================================
echo          AES TEST (binary-safe)
echo ===========================================
echo [INFO] Using CryptoCore: "%CRYPTOCORE%"
echo [INFO] Using OpenSSL:   "%OPENSSL%"
echo.

for %%S in (%SIZES%) do (
    echo -------------------------------------------
    echo Testing size %%S bytes
    echo -------------------------------------------

    rem --- create binary plaintext with OpenSSL (cryptographically random, no CRLF)
    "%OPENSSL%" rand %%S > plain.bin

    for %%M in (%MODES%) do (
        echo   -- mode: %%M

        rem prepare IV args only for non-ecb
        if /I "%%M"=="ecb" (
            set "IV_ARG_CCR="
            set "IV_ARG_OSSL="
        ) else (
            rem generate 16-byte IV hex (lowercase)
            "%OPENSSL%" rand -hex 16 > iv.hex
            set /p IV_HEX=<iv.hex
            rem strip potential CR/LF (safe)
            for /f "delims=" %%A in ('type iv.hex') do set "IV_HEX=%%A"
            set "IV_ARG_CCR=--iv !IV_HEX!"
            set "IV_ARG_OSSL=-iv !IV_HEX!"
        )

        rem -------------------
        rem CryptoCore encrypt -> crypt.bin
        rem -------------------
        if "%SHOW_WEAK%"=="1" (
            "%CRYPTOCORE%" --algorithm aes --mode %%M --encrypt --key %KEY% !IV_ARG_CCR! --input plain.bin --output crypt.bin >crypt_stdout.log 2>crypt_stderr.log
        ) else (
            "%CRYPTOCORE%" --algorithm aes --mode %%M --encrypt --key %KEY% !IV_ARG_CCR! --input plain.bin --output crypt.bin >crypt_stdout.log 2>nul
        )

        rem -------------------
        rem OpenSSL encrypt -> crypt_ref.bin
        rem -------------------
        "%OPENSSL%" enc -aes-128-%%M -K %KEY% !IV_ARG_OSSL! -nosalt -in plain.bin -out crypt_ref.bin 2>nul

        rem compare ciphertexts
        fc /b crypt.bin crypt_ref.bin >nul 2>nul
        if errorlevel 1 (
            echo     [FAIL] Encryption mismatch in %%M for size %%S
        ) else (
            echo     [OK] Encryption match
        )

        rem -------------------
        rem CryptoCore decrypt crypt.bin -> plain_out.bin
        rem -------------------
        if "%SHOW_WEAK%"=="1" (
            "%CRYPTOCORE%" --algorithm aes --mode %%M --decrypt --key %KEY% !IV_ARG_CCR! --input crypt.bin --output plain_out.bin >nul 2>crypt_decrypt_stderr.log
        ) else (
            "%CRYPTOCORE%" --algorithm aes --mode %%M --decrypt --key %KEY% !IV_ARG_CCR! --input crypt.bin --output plain_out.bin >nul 2>nul
        )

        rem compare plaintexts
        fc /b plain.bin plain_out.bin >nul 2>nul
        if errorlevel 1 (
            echo     [FAIL] Decryption mismatch in %%M for size %%S
        ) else (
            echo     [OK] Decryption match
        )

        rem cleanup per-mode small files
        if exist crypt.bin del /f /q crypt.bin >nul 2>nul
        if exist crypt_ref.bin del /f /q crypt_ref.bin >nul 2>nul
        if exist plain_out.bin del /f /q plain_out.bin >nul 2>nul
        if exist iv.hex del /f /q iv.hex >nul 2>nul
        if exist crypt_stdout.log del /f /q crypt_stdout.log >nul 2>nul
        if exist crypt_stderr.log del /f /q crypt_stderr.log >nul 2>nul
        if exist crypt_decrypt_stderr.log del /f /q crypt_decrypt_stderr.log >nul 2>nul
    )

    rem keep plain.bin between sizes (or regenerate next loop)
)

echo.
echo ===========================================
echo             TESTS COMPLETE
echo ===========================================
pause
