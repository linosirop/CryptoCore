@echo off
setlocal enabledelayedexpansion

echo ========================================
echo    FINAL CryptoCore Test - Sprint 2
echo ========================================
echo.

set "TEST_KEY=000102030405060708090a0b0c0d0e0f"
set "SUCCESS_COUNT=0"
set "TOTAL_TESTS=0"

set "CRYPTO_EXE=..\..\x64\Debug\CryptoCore.exe"

if not exist "%CRYPTO_EXE%" (
    echo ❌ ERROR: CryptoCore.exe not found!
    pause
    exit /b 1
)

echo ✅ Found CryptoCore.exe
echo.

:: Тестируем все режимы с правильными размерами данных
call :test_mode "ecb" "no_iv" "16" "exact"
call :test_mode "cbc" "with_iv" "24" "any"
call :test_mode "cfb" "with_iv" "33" "any" 
call :test_mode "ofb" "with_iv" "42" "any"
call :test_mode "ctr" "with_iv" "51" "any"

:: Итоги
echo.
echo ========================================
echo    FINAL TEST SUMMARY
echo ========================================
echo Tests passed: %SUCCESS_COUNT%/%TOTAL_TESTS%
if %SUCCESS_COUNT% == %TOTAL_TESTS% (
    echo.
    echo 🎉 CONGRATULATIONS! ALL TESTS PASSED!
    echo 🚀 Sprint 2 COMPLETED SUCCESSFULLY!
    echo.
) else (
    echo ❌ SOME TESTS FAILED!
)

del test*.txt test*.enc test*.dec 2>nul
pause
exit /b 0

:test_mode
set "MODE=%~1"
set "IV_TYPE=%~2"
set "SIZE=%~3"
set "EXACT=%~4"
set /a TOTAL_TESTS+=1

echo Testing %MODE% mode...

:: Создаем тестовые данные
if "%EXACT%"=="exact" (
    :: Для ECB создаем ТОЧНО 16 байт без лишних символов
    powershell -Command "[System.IO.File]::WriteAllBytes('test_input.txt', [byte[]](1..16))"
) else (
    :: Для остальных режимов используем текстовые данные
    powershell -Command "[System.IO.File]::WriteAllText('test_input.txt', 'A' * %SIZE%)"
)

:: Шифрование
"%CRYPTO_EXE%" --algorithm aes --mode %MODE% --encrypt --key %TEST_KEY% --input test_input.txt --output test_%MODE%.enc
if !errorlevel! neq 0 (
    echo ❌ %MODE% encryption failed!
    goto :cleanup
)

:: Дешифровка
if "%IV_TYPE%"=="with_iv" (
    "%CRYPTO_EXE%" --algorithm aes --mode %MODE% --decrypt --key %TEST_KEY% --input test_%MODE%.enc --output test_%MODE%.dec
) else (
    "%CRYPTO_EXE%" --algorithm aes --mode %MODE% --decrypt --key %TEST_KEY% --input test_%MODE%.enc --output test_%MODE%.dec
)

if !errorlevel! neq 0 (
    echo ❌ %MODE% decryption failed!
    goto :cleanup
)

:: Проверка результата
if "%EXACT%"=="exact" (
    :: Для ECB сравниваем бинарные файлы
    fc /b test_input.txt test_%MODE%.dec >nul
) else (
    :: Для остальных режимов сравниваем текстовые файлы
    fc test_input.txt test_%MODE%.dec >nul
)

if !errorlevel! equ 0 (
    echo ✅ %MODE% test PASSED
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ %MODE% test FAILED
)

:cleanup
del test_input.txt test_%MODE%.enc test_%MODE%.dec 2>nul
exit /b 0