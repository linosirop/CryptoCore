#include "cli_parser.h"
#include <iostream>
#include <map>
#include <sstream>
#include <cctype>
#include <cstdlib>

CliArgs parse_args(int argc, char* argv[]) {
    CliArgs args;
    std::map<std::string, std::string> flags;

    // Парсинг аргументов
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Пропускаем пустые аргументы
        if (arg.empty()) continue;

        // Обрабатываем аргументы с --
        if (arg.rfind("--", 0) == 0) {
            // Проверяем формат --key=value
            size_t eq_pos = arg.find('=');
            if (eq_pos != std::string::npos) {
                // Формат --key=value
                std::string key = arg.substr(2, eq_pos - 2);
                std::string value = arg.substr(eq_pos + 1);
                flags[key] = value;
            }
            else {
                // Формат --key value или --flag
                std::string key = arg.substr(2);

                // Проверяем, есть ли следующий аргумент и он не начинается с --
                if (i + 1 < argc) {
                    std::string next_arg = argv[i + 1];
                    if (next_arg.rfind("--", 0) != 0) {
                        // Следующий аргумент - значение
                        flags[key] = next_arg;
                        i++; // Пропускаем следующий аргумент
                    }
                    else {
                        // Следующий аргумент тоже ключ, значит текущий - флаг
                        flags[key] = "true";
                    }
                }
                else {
                    // Это последний аргумент, значит флаг
                    flags[key] = "true";
                }
            }
        }
        else {
            std::cerr << "[ERROR] Invalid arg format: " << arg << std::endl;
            std::cerr << "Use --key=value or --key value format" << std::endl;
            std::exit(1);
        }
    }

    // Валидация algorithm (CLI-2)
    if (flags.count("algorithm")) {
        args.algorithm = flags["algorithm"];
    }
    if (args.algorithm != "aes") {
        std::cerr << "[ERROR] Only 'aes' algorithm supported" << std::endl;
        std::exit(1);
    }

    // Валидация mode (CLI-2)
    if (flags.count("mode")) {
        args.mode = flags["mode"];
    }
    if (args.mode != "ecb") {
        std::cerr << "[ERROR] Only 'ecb' mode supported" << std::endl;
        std::exit(1);
    }

    // Валидация encrypt/decrypt (CLI-2, CLI-4)
    args.encrypt = flags.count("encrypt");
    args.decrypt = flags.count("decrypt");
    if ((args.encrypt && args.decrypt) || (!args.encrypt && !args.decrypt)) {
        std::cerr << "[ERROR] Specify exactly one: --encrypt or --decrypt" << std::endl;
        std::exit(1);
    }

    // Валидация key (CLI-2, CLI-3)
    if (flags.count("key")) {
        args.key_hex = flags["key"];
        if (args.key_hex.length() != 32) {
            std::cerr << "[ERROR] --key must be 32 hex characters (16 bytes)" << std::endl;
            std::exit(1);
        }
        for (char c : args.key_hex) {
            if (!std::isxdigit(static_cast<unsigned char>(c))) {
                std::cerr << "[ERROR] --key must contain only hexadecimal characters" << std::endl;
                std::exit(1);
            }
        }
    }
    else {
        std::cerr << "[ERROR] --key is required" << std::endl;
        std::exit(1);
    }

    // Валидация input (CLI-2)
    if (flags.count("input")) {
        args.input_file = flags["input"];
    }
    else {
        std::cerr << "[ERROR] --input is required" << std::endl;
        std::exit(1);
    }

    // Обработка output (CLI-2, CLI-5)
    if (flags.count("output")) {
        args.output_file = flags["output"];
    }
    else {
        // Генерация имени файла по умолчанию
        args.output_file = args.input_file + (args.encrypt ? ".enc" : ".dec");
    }

    return args;
}