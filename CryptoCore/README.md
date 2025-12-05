CryptoCore — Documentation (Text Version)
1. Overview

CryptoCore — консольное приложение, реализующее две независимые криптографические подсистемы:

Хеширование:

SHA-256

SHA3-256

Симметричное шифрование на основе AES-128:

ECB

CBC

CFB

OFB

CTR

Все алгоритмы реализованы вручную: AES-128 primitive, режимы шифрования, PKCS#7, потоковое хеширование SHA-256 и SHA3-256.

2. Build Requirements

C++17

OpenSSL 3.x (только для тестовых BAT-скриптов)

Windows

3. Command Line Interface Overview

CryptoCore поддерживает два основных режима:

dgst — хеширование файлов

aes — шифрование / дешифрование AES-128

4. Hashing (dgst)
4.1. Формат запуска
CryptoCore.exe dgst --algorithm <sha256|sha3-256> --input <file> [--output <file>]


Обязательные параметры:

dgst — включает режим хеширования

--algorithm — либо sha256, либо sha3-256

--input — путь к файлу

--output — необязательно, если не указано — запись в hash.bin

4.2. Формат вывода (строгий)

Программа выводит:

<hex_digest> <input_filename>
[INFO] Digest completed.


Если указан файл вывода, в него записывается точно такая же строка.

4.3. Потоковая обработка

Хеширование выполняется без загрузки всего файла в память:

файл читается блоками по 64 КБ

SHA-256 корректно применяет padding 1…0… + 64-битная длина

SHA3-256 использует Keccak-f[1600] с абсорбцией и squeez-фазой

4.4. Примеры

SHA-256:

CryptoCore.exe dgst --algorithm sha256 --input abc.txt


SHA3-256:

CryptoCore.exe dgst --algorithm sha3-256 --input file.bin

4.5. Соответствие тестам

Программа проходит:

NIST SHA-256 тесты

NIST SHA3-256 тесты

длинный тест "abcdbc...nopq"

тест 1 000 000 символов "a"

5. AES Encryption / Decryption
5.1. Формат запуска
CryptoCore.exe --algorithm aes --mode <ecb|cbc|cfb|ofb|ctr> (--encrypt or --decrypt)
               --key <hex> [--iv <hex>] --input <file> --output <file>

5.2. Обязательные параметры

--algorithm aes

--mode — один из пяти режимов

--encrypt или --decrypt — обязательно одно

--key — 128-битный ключ в hex-формате (ровно 32 символа)

--input — входной файл

--output — выходной файл

Дополнительно:

--iv обязателен для CBC, CFB, OFB, CTR

в режиме ECB IV не используется

5.3. Проверка ключей и IV

Программа проверяет:

hex-строка должна иметь чётную длину

hex-строка ключа и IV должна содержать ровно 32 символа

при ошибке выводится:

[ERROR] Hex string length must be even

[ERROR] Invalid hex character

[ERROR] Key/IV must be exactly 16 bytes

5.4. Padding

PKCS#7 применяется для режимов ECB и CBC

CFB, OFB, CTR не используют padding

5.5. Реализация AES

Реализовано вручную:

AES-128 block encryption/decryption

режимы ECB, CBC, CFB, OFB, CTR

XOR цепочки и обработка состояния

PKCS#7 padding

OpenSSL не используется в реализации алгоритмов.

5.6. Примеры

ECB encrypt:

CryptoCore.exe --algorithm aes --mode ecb --encrypt --key 00112233445566778899AABBCCDDEEFF --input plain.bin --output enc.bin


CBC decrypt:

CryptoCore.exe --algorithm aes --mode cbc --decrypt --key 001122... --iv 0102030405... --input enc.bin --output dec.bin

5.7. Валидность

Реализация проверена BAT-скриптом, который сравнивает:

AES-ECB с OpenSSL

AES-CBC с OpenSSL

AES-CFB, OFB, CTR с OpenSSL

корректность обратного преобразования

корректность padding

Все тесты проходят успешно.

6. Error Handling

Все ошибки имеют строгий формат:

[ERROR] <описание>


Обрабатываются:

неверные аргументы

ошибки открытия файла

некорректные hex-строки

ошибки padding

отсутствие необходимых параметров

7. Known Limitations

Поддерживается только AES-128

Хеш-алгоритмы ограничены SHA-256 и SHA3-256

Padding — только PKCS#7

IV не генерируется автоматически (должен быть указан пользователем)

8. Test Suite

В комплект входят:

NIST-SHA-TEST.bat — проверка SHA-256 и SHA3-256

NIST-AES-TEST.bat — сравнение всех AES режимов с OpenSSL

автоматическая генерация:

пустой строки

файла "abc"

NIST long test

файла из 1 000 000 символов 'a'

Все тесты завершаются успешным прохождением.

9. Conclusion

CryptoCore полностью реализует:

SHA-256

SHA3-256

AES-128 во всех пяти режимах

потоковое хеширование

PKCS#7 padding

строгий CLI

корректную обработку ошибок

прохождение NIST и OpenSSL-совместимых тестов

Документация и реализация соответствуют требованиям Sprint 4