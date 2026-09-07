/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <chrono>
#include <format>
#include <iostream>

#include "MacroHelper.h"

#define ANSI_RESET "\033[0m"
#define ANSI_RED "\033[0;31m"
#define ANSI_YELLOW "\033[0;33m"
#define ANSI_GREEN "\033[0;32m"
#define ANSI_BLUE "\033[0;34m"
#define ANSI_WHITE "\033[0;37m"
#define ANSI_RED_BACKGROUND "\033[41m"

constexpr std::string log_strip_file_path(const std::string& path) {
	const size_t pos = path.find("src/");
	return pos != std::string::npos ? path.substr(pos + 4) : path;
}

#ifdef VG_DEBUG
#define LOG_COMMON(colourCode, type, message, ...) std::cout << colourCode << "[" << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::round<std::chrono::duration<uint64_t, std::ratio<1, 100>>>(std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()).get_local_time())) << "] [" << type << "]" << ANSI_RESET << " [" << MACRO_TO_STRING(LOG_PROJECT_NAME) << ":" << log_strip_file_path(__FILE__) << ":" << std::to_string(__LINE__) << "] " << std::format(message __VA_OPT__(,) __VA_ARGS__) << std::endl
#else
#define LOG_COMMON(colourCode, type, message, ...) std::cout << colourCode << "[" << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::round<std::chrono::duration<uint64_t, std::ratio<1, 100>>>(std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()).get_local_time())) << "] [" << type << "]" << ANSI_RESET << " [" << MACRO_TO_STRING(LOG_PROJECT_NAME) << "] " << std::format(message __VA_OPT__(,) __VA_ARGS__) << std::endl
#endif

#define LOG_FATAL(message, ...) LOG_COMMON(ANSI_WHITE << ANSI_RED_BACKGROUND, "FATAL", message __VA_OPT__(,) __VA_ARGS__)

#define LOG_ERROR(message, ...) LOG_COMMON(ANSI_RED, "ERROR", message __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARNING(message, ...) LOG_COMMON(ANSI_YELLOW, "WARNING", message __VA_OPT__(,) __VA_ARGS__)

#define LOG_INFO(message, ...) LOG_COMMON(ANSI_GREEN, "INFO", message __VA_OPT__(,) __VA_ARGS__)

#ifdef VG_DEBUG
#define LOG_DEBUG(message, ...) LOG_COMMON(ANSI_BLUE, "DEBUG", message __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_DEBUG(message, ...) do {if constexpr (false) {std::string s = std::format(message __VA_OPT__(,) __VA_ARGS__);}} while (0)
#endif