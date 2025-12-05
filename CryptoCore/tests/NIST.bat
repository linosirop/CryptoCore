@echo off
setlocal enabledelayedexpansion

echo ===========================================
echo    FULL NIST SHA-256 / SHA3-256 TEST SUITE
echo ===========================================

set EXE=C:\Users\User\source\repos\CryptoCore\x64\Debug\CryptoCore.exe

if not exist "%EXE%" (
    echo [ERROR] CryptoCore not found at:
    echo   %EXE%
    pause
    exit /b
)

rem ----------------------------------------------------
rem   CREATE TEST FILES EXACTLY AS BYTES
rem ----------------------------------------------------

echo [INFO] Creating test files...

rem empty.txt   → 0 bytes
powershell -Command "[IO.File]::WriteAllBytes('empty.txt', @())"

rem abc.txt  → exactly 3 bytes: 61 62 63
powershell -Command "[IO.File]::WriteAllBytes('abc.txt',[Text.Encoding]::ASCII.GetBytes('abc'))"

rem long1.txt → 'abcdbcde...nopq'
powershell -Command "[IO.File]::WriteAllText('long1.txt','abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq')"

rem million.txt → 1 000 000 bytes of 'a'
echo [INFO] Generating 1,000,000-byte test file (this may take a sec)...
powershell -Command "[IO.File]::WriteAllBytes('million.txt', (,[byte]([char]'a')) * 1000000)"


rem ====================================================
rem               SHA-256 TESTS
rem ====================================================

echo.
echo -------- SHA-256 TESTS --------

rem SHA-256("")
set EXPECT1=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 empty.txt
"%EXE%" dgst --algorithm sha256 --input empty.txt > out1.txt
set /p GOT1=<out1.txt

if "!GOT1!"=="!EXPECT1!" (
    echo [OK] empty string
) else (
    echo [FAIL] empty string
    echo Expected:
    echo   !EXPECT1!
    echo Got:
    echo   !GOT1!
)

rem SHA-256("abc")
set EXPECT2=ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad abc.txt
"%EXE%" dgst --algorithm sha256 --input abc.txt > out2.txt
set /p GOT2=<out2.txt

if "!GOT2!"=="!EXPECT2!" (
    echo [OK] abc
) else (
    echo [FAIL] abc
    echo Expected:
    echo   !EXPECT2!
    echo Got:
    echo   !GOT2!
)

rem SHA-256(long NIST test)
set EXPECT3=248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1 long1.txt
"%EXE%" dgst --algorithm sha256 --input long1.txt > out3.txt
set /p GOT3=<out3.txt

if "!GOT3!"=="!EXPECT3!" (
    echo [OK] NIST long test
) else (
    echo [FAIL] NIST long test
    echo Expected:
    echo   !EXPECT3!
    echo Got:
    echo   !GOT3!
)

rem SHA-256(1,000,000 × 'a')
set EXPECT4=cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0 million.txt
"%EXE%" dgst --algorithm sha256 --input million.txt > out4.txt
set /p GOT4=<out4.txt

if "!GOT4!"=="!EXPECT4!" (
    echo [OK] million a
) else (
    echo [FAIL] million a
    echo Expected:
    echo   !EXPECT4!
    echo Got:
    echo   !GOT4!
)



rem ====================================================
rem               SHA3-256 TESTS
rem ====================================================

echo.
echo -------- SHA3-256 TESTS --------

rem SHA3-256("")
set EXPECT5=a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a empty.txt
"%EXE%" dgst --algorithm sha3-256 --input empty.txt > out5.txt
set /p GOT5=<out5.txt

if "!GOT5!"=="!EXPECT5!" (
    echo [OK] SHA3 empty
) else (
    echo [FAIL] SHA3 empty
    echo Expected:
    echo   !EXPECT5!
    echo Got:
    echo   !GOT5!
)

rem SHA3-256("abc")
set EXPECT6=3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532 abc.txt
"%EXE%" dgst --algorithm sha3-256 --input abc.txt > out6.txt
set /p GOT6=<out6.txt

if "!GOT6!"=="!EXPECT6!" (
    echo [OK] SHA3 abc
) else (
    echo [FAIL] SHA3 abc
    echo Expected:
    echo   !EXPECT6!
    echo Got:
    echo   !GOT6!
)


echo.
echo ===========================================
echo            ALL TESTS FINISHED
echo ===========================================
pause
