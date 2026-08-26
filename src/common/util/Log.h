#pragma once

#include <chrono>
#include <format>
#include <iostream>

#define ANSI_RESET "\033[0m"
#define ANSI_RED "\033[0;31m"
#define ANSI_YELLOW "\033[0;33m"
#define ANSI_GREEN "\033[0;32m"
#define ANSI_BLUE "\033[0;34m"
#define ANSI_WHITE "\033[0;37m"
#define ANSI_RED_BACKGROUND "\033[41m"

#define LOG_STRINGIFY(x) #x
#define LOG_TO_STRING(x) LOG_STRINGIFY(x)

constexpr std::string log_strip_file_path(const std::string& path) {
	const size_t pos = path.find("src/");
	return pos != std::string::npos ? path.substr(pos + 4) : path;
}

#ifdef VG_DEBUG
#define LOG_COMMON(colourCode, type, message, ...) std::cout << colourCode << "[" << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()).get_local_time())) << "] [" << type << "]" << ANSI_RESET << " [" << LOG_TO_STRING(LOG_PROJECT_NAME) << ":" << log_strip_file_path(__FILE__) << ":" << std::to_string(__LINE__) << "] " << std::format(message __VA_OPT__(,) __VA_ARGS__) << std::endl
#else
#define LOG_COMMON(colourCode, type, message, ...) std::cout << colourCode << "[" << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()).get_local_time())) << "] [" << type << "]" << ANSI_RESET << " [" << LOG_TO_STRING(LOG_PROJECT_NAME) << "] " << std::format(message __VA_OPT__(,) __VA_ARGS__) << std::endl
#endif

#define LOG_FATAL(message, ...) LOG_COMMON(ANSI_WHITE << ANSI_RED_BACKGROUND, "FATAL", message __VA_OPT__(,) __VA_ARGS__)

#define LOG_ERROR(message, ...) LOG_COMMON(ANSI_RED, "ERROR", message __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARNING(message, ...) LOG_COMMON(ANSI_YELLOW, "WARNING", message __VA_OPT__(,) __VA_ARGS__)

#define LOG_INFO(message, ...) LOG_COMMON(ANSI_GREEN, "INFO", message __VA_OPT__(,) __VA_ARGS__)

#ifdef VG_DEBUG
#define LOG_DEBUG(message, ...) LOG_COMMON(ANSI_BLUE, "DEBUG", message __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_DEBUG(message, ...) do {} while (0)
#endif