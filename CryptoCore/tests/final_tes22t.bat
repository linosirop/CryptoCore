@echo off
echo COMPLETE ALL MODES TEST
echo.

set "KEY=000102030405060708090a0b0c0d0e0f"

echo Testing all modes with proper binary data...
echo.

:: Test ECB with exact 16 bytes
powershell -Command "[System.IO.File]::WriteAllBytes('test.bin', [byte[]](1..16))"
..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode ecb --encrypt --key %KEY% --input test.bin --output test_ecb.enc
..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode ecb --decrypt --key %KEY% --input test_ecb.enc --output test_ecb.dec
fc /b test.bin test_ecb.dec >nul && echo ✅ ECB test PASSED || echo ❌ ECB test FAILED

:: Test other modes
..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode cbc --encrypt --key %KEY% --input test.bin --output test_cbc.enc
..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode cbc --decrypt --key %KEY% --input test_cbc.enc --output test_cbc.dec
fc /b test.bin test_cbc.dec >nul && echo ✅ CBC test PASSED || echo ❌ CBC test FAILED

..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode cfb --encrypt --key %KEY% --input test.bin --output test_cfb.enc
..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode cfb --decrypt --key %KEY% --input test_cfb.enc --output test_cfb.dec
fc /b test.bin test_cfb.dec >nul && echo ✅ CFB test PASSED || echo ❌ CFB test FAILED

..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode ofb --encrypt --key %KEY% --input test.bin --output test_ofb.enc
..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode ofb --decrypt --key %KEY% --input test_ofb.enc --output test_ofb.dec
fc /b test.bin test_ofb.dec >nul && echo ✅ OFB test PASSED || echo ❌ OFB test FAILED

..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode ctr --encrypt --key %KEY% --input test.bin --output test_ctr.enc
..\..\x64\Debug\CryptoCore.exe --algorithm aes --mode ctr --decrypt --key %KEY% --input test_ctr.enc --output test_ctr.dec
fc /b test.bin test_ctr.dec >nul && echo ✅ CTR test PASSED || echo ❌ CTR test FAILED

del test*.bin test*.enc test*.dec
echo.
echo Test completed!
pause