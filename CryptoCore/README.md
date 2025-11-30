\# CryptoCore

CLI-инструмент для криптографических операций (Sprint 1: AES-128 ECB).



Реализованные возможности

Алгоритм: AES-128 (128-битный ключ, 128-битные блоки)



Режим: ECB (Electronic Codebook)



Дополнение: PKCS#7



Формат ключа: 32-символьная hex-строка (16 байт)



Выполненные требования Sprint 1

Project Structure \& Repository Hygiene (STR)

STR-1: Git репозиторий



STR-2: README с инструкциями



STR-3: Система сборки (CMake/Visual Studio)



STR-4: Логическая структура папок



Command-Line Interface (CLI)

CLI-1: Запускается как cryptocore



CLI-2: Поддержка всех требуемых аргументов



CLI-3: Ключ в hex-формате



CLI-4: Валидация аргументов



CLI-5: Автогенерация имени выходного файла



Core Cryptographic Implementation (CRY)

CRY-1: AES-128



CRY-2: Использование OpenSSL



CRY-3: Логика ECB режима



CRY-4: PKCS#7 padding



CRY-5: Корректная работа с бинарными файлами



File I/O

IO-1: Чтение входных файлов



IO-2: Запись выходных файлов



IO-3: Обработка ошибок файловой системы



Зависимости

Язык: C++17



Компилятор: Visual Studio 2022+



Библиотеки: OpenSSL 3.x



Система сборки: CMake 3.20+ или Visual Studio Project



Инструкции по сборке

Способ 1: Visual Studio

Откройте CryptoCore.sln в Visual Studio 2022+



Сборка → Собрать решение (Ctrl+Shift+B)



Исполняемый файл будет создан в x64/Debug/ или x64/Release/



Способ 2: CMake

bash

mkdir build

cd build

cmake ..

cmake --build . --config Release

Настройка OpenSSL

Проект использует vcpkg для управления зависимостями.



Использование

Синтаксис:

bash

\# Формат с пробелом

cryptocore --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input plaintext.txt --output ciphertext.bin



\# Формат с =

cryptocore --algorithm=aes --mode=ecb --decrypt --key=000102030405060708090a0b0c0d0e0f --input=ciphertext.bin --output=decrypted.txt

Аргументы:

--algorithm ALGORITHM - алгоритм шифрования (только aes)



--mode MODE - режим работы (только ecb)



--encrypt или --decrypt - операция (обязательно один)



--key KEY - ключ (32 hex-символа, 16 байт)



--input FILE - входной файл (обязательно)



--output FILE - выходной файл (опционально)



Примеры:

bash

\# Шифрование

cryptocore --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input document.txt --output document.enc



\# Дешифрование

cryptocore --algorithm aes --mode ecb --decrypt --key 000102030405060708090a0b0c0d0e0f --input document.enc --output document\_decrypted.txt

Тестирование

Round-trip тест:

bash

echo "Test message" > test.txt

cryptocore --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input test.txt --output encrypted.bin

cryptocore --algorithm aes --mode ecb --decrypt --key 000102030405060708090a0b0c0d0e0f --input encrypted.bin --output decrypted.txt

fc test.txt decrypted.txt

Структура проекта

text

CryptoCore/

├── src/

│   ├── main.cpp

│   ├── cli\_parser.cpp

│   ├── file\_io.cpp

│   └── modes/

│       └── ecb.cpp

├── include/

│   ├── cli\_parser.h

│   ├── file\_io.h

│   └── modes/

│       └── ecb.h

├── tests/

├── CMakeLists.txt

└── README.md

