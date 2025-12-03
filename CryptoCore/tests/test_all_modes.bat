@echo off
setlocal enabledelayedexpansion

echo ===========================================
echo          AES TORTURE TEST START
echo ===========================================

rem --- Paths ---
set "CRYPTOCORE=C:\Users\User\source\repos\CryptoCore\x64\Debug\CryptoCore.exe"
set "OPENSSL=C:\Program Files\OpenSSL-Win64\bin\openssl.exe"

echo [INFO] Using CryptoCore at "%CRYPTOCORE%"
echo [INFO] Using OpenSSL at "%OPENSSL%"
echo.

rem --- Parameters ---
set SIZES=16 31 32 47 48 63 64 100 256 1024 4096 16384 65536 131072 262144 524288 1048576
set MODES=ecb cbc cfb ofb ctr
set KEY=A1B2C3D4E5F60718293A4B5C6D7E8F90

for %%S in (%SIZES%) do (
    echo -------------------------------------------
    echo Testing size %%S bytes
    echo -------------------------------------------

    "%OPENSSL%" rand %%S > plain.bin

    for %%M in (%MODES%) do (
        echo   -- Testing mode: %%M

        rem prepare IV for non-ECB
        if /I NOT "%%M"=="ecb" (
            "%OPENSSL%" rand -hex 16 > iv.hex
            set /p IV_HEX=<iv.hex
            set "IV_ARG=--iv !IV_HEX!"
            set "IV_OSSL=-iv !IV_HEX!"
        ) else (
            set "IV_ARG="
            set "IV_OSSL="
        )

        rem ---- CryptoCore encrypt ----
        "%CRYPTOCORE%" --algorithm aes --mode %%M --encrypt --key %KEY% !IV_ARG! --input plain.bin --output crypt.bin >nul 2>cc_err_enc.txt

        rem ---- OpenSSL encrypt reference ----
        "%OPENSSL%" enc -aes-128-%%M -K %KEY% !IV_OSSL! -nosalt -in plain.bin -out crypt_ref.bin >nul 2>ossl_err_enc.txt

        fc /b crypt.bin crypt_ref.bin >nul
        if errorlevel 1 (
            echo     [FAIL] Encryption mismatch in %%M for size %%S
            echo     CryptoCore ERR:
            type cc_err_enc.txt
            echo     OpenSSL ERR:
            type ossl_err_enc.txt
        ) else (
            echo     [OK] Encryption match
        )

        rem ---- CryptoCore decrypt ----
        "%CRYPTOCORE%" --algorithm aes --mode %%M --decrypt --key %KEY% !IV_ARG! --input crypt.bin --output plain_out.bin >nul 2>cc_err_dec.txt

        fc /b plain.bin plain_out.bin >nul
        if errorlevel 1 (
            echo     [FAIL] Decryption mismatch in %%M for size %%S
            echo     CryptoCore ERR:
            type cc_err_dec.txt
        ) else (
            echo     [OK] Decryption match
        )

        del /f /q cc_err_enc.txt cc_err_dec.txt ossl_err_enc.txt iv.hex >nul 2>&1
        del /f /q crypt.bin crypt_ref.bin plain_out.bin >nul 2>&1
    )
    echo.
)

rem cleanup
del /f /q plain.bin >nul 2>&1

echo ===========================================
echo      ALL TORTURE TESTS FINISHED
echo ===========================================
pause
