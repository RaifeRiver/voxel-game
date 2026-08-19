#pragma once

#include <format>
#include <iostream>

#define ANSI_RESET "\033[0m"
#define ANSI_RED "\033[0;31m"
#define ANSI_YELLOW "\033[0;33m"
#define ANSI_GREEN "\033[0;32m"
#define ANSI_BLUE "\033[0;34m"

#define LOG_STRINGIFY(x) #x
#define LOG_TO_STRING(x) LOG_STRINGIFY(x)

constexpr std::string log_strip_file_path(const std::string& path) {
	const size_t pos = path.find("src/");
	return pos != std::string::npos ? path.substr(pos + 4) : path;
}

#ifdef VG_DEBUG
#define LOG_COMMON(colourCode, type, message, ...) std::cout << colourCode << "[" << type << "]" << ANSI_RESET << " [" << LOG_TO_STRING(LOG_PROJECT_NAME) << ":" << log_strip_file_path(__FILE__) << ":" << std::to_string(__LINE__) << "] " << std::format(message __VA_OPT__(,) __VA_ARGS__) << std::endl
#else
#define LOG_COMMON(colourCode, type, message, ...) std::cout << colourCode << "[" << type << "]" << ANSI_RESET << " [" << LOG_TO_STRING(LOG_PROJECT_NAME) << "] " << std::format(message __VA_OPT__(,) __VA_ARGS__) << std::endl
#endif

#define LOG_ERROR(message, ...) LOG_COMMON(ANSI_RED, "ERROR", message __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARNING(message, ...) LOG_COMMON(ANSI_YELLOW, "WARNING", message __VA_OPT__(,) __VA_ARGS__)

#define LOG_INFO(message, ...) LOG_COMMON(ANSI_GREEN, "INFO", message __VA_OPT__(,) __VA_ARGS__)

#ifdef VG_DEBUG
#define LOG_DEBUG(message, ...) LOG_COMMON(ANSI_BLUE, "DEBUG", message __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_DEBUG(message, ...)
#endif