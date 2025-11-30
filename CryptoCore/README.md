\# CryptoCore



CLI-инструмент для криптографических операций (Sprint 1: AES-128 ECB).



\## Реализованные возможности



\- \*\*Алгоритм\*\*: AES-128 (128-битный ключ, 128-битные блоки)

\- \*\*Режим\*\*: ECB (Electronic Codebook)

\- \*\*Дополнение\*\*: PKCS#7

\- \*\*Формат ключа\*\*: 32-символьная hex-строка (16 байт)



\## Выполненные требования Sprint 1



\### Project Structure \& Repository Hygiene (STR)

\- STR-1: Git репозиторий

\- STR-2: README с инструкциями

\- STR-3: Система сборки (Visual Studio проект и CMake)

\- STR-4: Логическая структура папок



\### Command-Line Interface (CLI)

\- CLI-1: Запускается как `cryptocore`

\- CLI-2: Поддержка всех требуемых аргументов

\- CLI-3: Ключ в hex-формате

\- CLI-4: Валидация аргументов

\- CLI-5: Автогенерация имени выходного файла



\### Core Cryptographic Implementation (CRY)

\- CRY-1: AES-128

\- CRY-2: Использование OpenSSL

\- CRY-3: Логика ECB режима

\- CRY-4: PKCS#7 padding

\- CRY-5: Корректная работа с бинарными файлами



\### File I/O

\- IO-1: Чтение входных файлов

\- IO-2: Запись выходных файлов

\- IO-3: Обработка ошибок файловой системы



\### Testing \& Verification

\- TEST-1: Round-trip тестирование (шифрование + дешифрование)

\- TEST-2: Автоматический тестовый скрипт

\- TEST-3: Сравнение с оригинальным файлом



\## Зависимости



\- \*\*Язык\*\*: C++17

\- \*\*Компилятор\*\*: Visual Studio 2022+ или совместимый

\- \*\*Библиотеки\*\*: OpenSSL 3.x

\- \*\*Система сборки\*\*: CMake 3.20+ или Visual Studio Project



\## Инструкции по сборке



\### Способ 1: Visual Studio

1\. Откройте `CryptoCore.sln` в Visual Studio 2022+

2\. Сборка -> Собрать решение (Ctrl+Shift+B)

3\. Исполняемый файл будет создан в `x64/Debug/` или `x64/Release/`



\### Способ 2: CMake

```bash

cd CryptoCore

mkdir build

cd build

cmake ..

cmake --build . --config Release



Примеры:

bash

\# Шифрование

cryptocore --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input document.txt --output document.enc



\# Дешифрование

cryptocore --algorithm aes --mode ecb --decrypt --key 000102030405060708090a0b0c0d0e0f --input document.enc --output document\_decrypted.txt



\# Автогенерация имени файла

cryptocore --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input data.bin

\# Создаст data.bin.enc



Тестирование

Ручное тестирование:

bash

echo "Hello CryptoCore!" > test.txt

cryptocore --algorithm aes --mode ecb --encrypt --key 000102030405060708090a0b0c0d0e0f --input test.txt --output encrypted.bin

cryptocore --algorithm aes --mode ecb --decrypt --key 000102030405060708090a0b0c0d0e0f --input encrypted.bin --output decrypted.txt

fc test.txt decrypted.txt

\# Должен показать, что файлы идентичны

Автоматический тест:

Запустите tests/final\_test.bat для автоматического round-trip тестирования.



Структура проекта

text

CryptoCore/

├── src/                 # Исходный код

│   ├── main.cpp        # Точка входа

│   ├── cli\_parser.cpp  # Парсер аргументов

│   ├── file\_io.cpp     # Работа с файлами

│   └── modes/

│       └── ecb.cpp     # Реализация ECB

├── include/            # Заголовочные файлы

│   ├── cli\_parser.h

│   ├── file\_io.h

│   └── modes/

│       └── ecb.h

├── tests/              # Тесты

│   ├── test\_s1.bat    # Автоматический тест

│   └── final\_test.bat # Гарантированный тест

├── CMakeLists.txt      # Конфигурация CMake

├── CryptoCore.vcxproj  # Visual Studio проект

└── README.md          # Документация

Проверка работоспособности

Проект был протестирован на:



Сборка через Visual Studio 2022



Сборка через CMake



Round-trip тестирование (шифрование + дешифрование)



Автоматический тестовый скрипт



Обработка ошибок ввода



Валидация аргументов командной строки

