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

10. Message Authentication Code (HMAC) — Sprint 5

В рамках Sprint 5 в CryptoCore реализована подсистема обеспечения аутентичности и целостности данных на основе HMAC.

Поддерживается:

HMAC-SHA256

Потоковая обработка файлов

Проверка (verification) ранее вычисленного HMAC

Реализация выполнена вручную, без использования OpenSSL или сторонних криптографических библиотек.

11. HMAC (dgst --hmac)
11.1. Формат запуска
CryptoCore.exe dgst --algorithm sha256 --hmac --key <hex_key> --input <file>


Дополнительно поддерживаются:

--verify <file>
--output <file>

11.2. Обязательные параметры

При использовании --hmac:

dgst — режим хеширования / MAC

--algorithm sha256 — единственный поддерживаемый алгоритм для HMAC

--hmac — включает режим HMAC

--key — обязательный параметр, ключ в hex-формате

--input — входной файл

Если параметр --key отсутствует, программа завершает работу с ошибкой:

[ERROR] --key required for HMAC

11.3. Формат вывода (строгий)

При успешном вычислении HMAC программа выводит:

<hex_hmac> <input_filename>
[INFO] HMAC completed.


Если указан --output, в файл записывается точно такая же строка.

11.4. Проверка HMAC (--verify)

Для проверки ранее вычисленного HMAC используется флаг --verify:

CryptoCore.exe dgst --algorithm sha256 --hmac --key <hex_key> --input <file> --verify <hmac_file>


Поведение:

программа вычисляет HMAC для входного файла

считывает ожидаемое значение HMAC из файла проверки

сравнивает значения

Файл проверки должен содержать строку формата:

<hex_hmac> <filename>


Имя файла игнорируется, используется только HMAC.

11.5. Результаты проверки

При успешной проверке:

[OK] HMAC verification successful


Код возврата: 0

При ошибке (изменён файл или неверный ключ):

[ERROR] HMAC verification failed


Код возврата: ненулевой

12. Реализация HMAC
12.1. Спецификация

HMAC реализован строго по RFC 2104:

HMAC(K, m) = H((K ⊕ opad) ∥ H((K ⊕ ipad) ∥ m))


Где:

H — SHA-256 (реализация из Sprint 4)

ipad = 0x36 × 64

opad = 0x5c × 64

12.2. Обработка ключей

Поддерживаются ключи произвольной длины:

если длина ключа > 64 байта — ключ предварительно хешируется

если длина ключа < 64 байта — дополняется нулями

если длина ключа = 64 байта — используется напрямую

12.3. Потоковая обработка

HMAC вычисляется без загрузки всего файла в память:

входной файл читается блоками

используется потоковый интерфейс SHA-256

память остаётся константной независимо от размера файла

13. Тестирование HMAC
13.1. Known-Answer Tests (RFC 4231)

Реализация проходит официальные тесты RFC 4231, включая:

Test Case 1 — ключ из 0x0b, сообщение "Hi There"

Test Case 2 — ключ "Jefe"

Test Case 3–4 — длинные ключи и данные

Результаты совпадают бит-в-бит с эталонными значениями.

13.2. Проверка целостности

Поддерживаются тесты:

обнаружение изменения файла

обнаружение использования неверного ключа

проверка пустого файла

проверка файлов, превышающих доступную память

Все проверки завершаются корректным кодом возврата.

13.3. Test Suite

В проект включён BAT-скрипт Sprint 5, выполняющий:

RFC 4231 тесты

генерацию и проверку HMAC

tamper detection (изменение файла)

проверку неверного ключа

тесты ключей разной длины

тесты пустых и больших файлов

Все тесты завершаются успешным прохождением.

Sprint 6 — Аутентифицированное шифрование (AEAD)
Общее описание

В шестом спринте проект CryptoCore был расширен поддержкой
аутентифицированного шифрования (AEAD) — то есть одновременного обеспечения:

конфиденциальности (шифрование),

целостности и подлинности данных (аутентификация).

Реализованы два подхода:

AES-GCM — стандартный режим AEAD (NIST SP 800-38D)

Encrypt-then-MAC (EtM) — композиция AES-CTR + HMAC-SHA256

Ключевое свойство:
➡ при любой ошибке аутентификации расшифрование не выполняется вообще
(катастрофический отказ).

Поддерживаемые режимы AEAD
1. AES-GCM (Galois/Counter Mode)

Шифрование: AES в режиме CTR

Аутентификация: GHASH по полю GF(2¹²⁸)

Длина тега: 16 байт (128 бит)

Nonce: 12 байт (рекомендовано NIST)

Формат выходного файла:

Nonce (12 байт) || Ciphertext || Tag (16 байт)

2. Encrypt-then-MAC (AES-CTR + HMAC-SHA256)

Шифрование: AES-CTR

Аутентификация: HMAC-SHA256

MAC считается по: Ciphertext || AAD

Используется разделение ключей:

K_enc — для шифрования

K_mac — для MAC

Формат выходного файла:

IV (16 байт) || Ciphertext || HMAC Tag (32 байта)

Associated Authenticated Data (AAD)

AAD (ассоциированные данные) — это данные, которые:

не шифруются,

но участвуют в аутентификации.

Примеры:

заголовки,

метаданные,

служебная информация,

контекст протокола.

Если AAD при расшифровании отличается от исходного —
➡ аутентификация проваливается, plaintext не выдаётся.

AAD передаётся через CLI в виде hex-строки.

Использование CLI
AES-GCM: шифрование с AAD
cryptocore --algorithm aes --mode gcm --encrypt \
--key 00112233445566778899aabbccddeeff \
--input plain.txt \
--output gcm.bin \
--aad aabbccddeeff

AES-GCM: расшифрование с корректным AAD
cryptocore --algorithm aes --mode gcm --decrypt \
--key 00112233445566778899aabbccddeeff \
--input gcm.bin \
--output decrypted.txt \
--aad aabbccddeeff


Результат:

[SUCCESS] GCM decryption completed

AES-GCM: неверный AAD
cryptocore --algorithm aes --mode gcm --decrypt \
--key 00112233445566778899aabbccddeeff \
--input gcm.bin \
--output fail.txt \
--aad deadbeef


Результат:

[ERROR] Authentication failed


выходной файл не создаётся

код завершения ≠ 0

Encrypt-then-MAC: пример
Шифрование
cryptocore --algorithm aes --mode etm --encrypt \
--key 00112233445566778899aabbccddeeff \
--input plain.txt \
--output etm.bin \
--aad aabbcc

Расшифрование
cryptocore --algorithm aes --mode etm --decrypt \
--key 00112233445566778899aabbccddeeff \
--input etm.bin \
--output etm_dec.txt \
--aad aabbcc

Свойства безопасности
Катастрофический отказ

Для GCM и Encrypt-then-MAC:

если тег не совпадает:

plaintext не выводится

файл результата не создаётся

программа немедленно завершает работу

Это защищает от:

подмены данных,

bit-flipping атак,

атак на целостность.

Nonce в GCM

Для каждого шифрования генерируется случайный nonce

Повтор nonce с тем же ключом — критическая уязвимость

Используется размер 12 байт (рекомендация NIST)

Реализация

GCM реализован с нуля

GHASH в поле GF(2¹²⁸)

Неприводимый многочлен:

x¹²⁸ + x⁷ + x² + x + 1


Сравнение тегов — в константное время

Поддержка:

произвольной длины plaintext

произвольной длины AAD

Тестирование (Sprint 6)

Проведены тесты:

корректность шифрование → расшифрование

подмена AAD

подмена ciphertext

повторное шифрование (разные nonce)

пустой AAD

большой AAD

Encrypt-then-MAC (все сценарии отказа)

Все требования Sprint 6 выполнены.
## Sprint 7 — Key Derivation Functions (PBKDF2 + Key Hierarchy)

### Goal
Добавлены функции безопасного получения ключей:
- **PBKDF2-HMAC-SHA256** — получение ключа из пароля (key stretching + salt + iterations)
- **Key hierarchy (HMAC-based derive_key)** — детерминированное получение нескольких независимых ключей из master key по разным `context`

---

## Команда `derive` (PBKDF2)

### Синтаксис
```bash
cryptocore derive --password "PASSWORD" [--salt SALT_HEX] [--iterations COUNT] [--length BYTES] [--algorithm pbkdf2] [--output FILE]

