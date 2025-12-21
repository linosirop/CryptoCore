\# CryptoCore Development Guide



\## Project layout

\- include/

\- src/

\- CryptoCore.cpp (CLI entry)

\- docs/

\- tests/



\## Build matrix

\- Windows MSVC

\- Windows MinGW (optional)

\- Linux (optional)



\## How to run tests

\- ctest (CMake)

\- python runner (integration)



\## Coding style \& documentation rules

\- Every public function: header comment explaining inputs/outputs/errors

\- Use consistent naming \& validation

\- Avoid logging secrets



\## Security checklist (dev)

\- validate inputs (key size, iv size, tag size)

\- zeroize sensitive buffers after use

\- constant-time compare for tags/HMAC where possible



