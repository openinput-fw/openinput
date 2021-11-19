/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#define LOG_LEVEL_OFF	0 /* Logging is disabled */
#define LOG_LEVEL_ERROR 1 /* Indicates an error condition */
#define LOG_LEVEL_WARN	2 /* Indicates a warning condition */
#define LOG_LEVEL_INFO	3 /* Informational messages */
#define LOG_LEVEL_DEBUG 4 /* Debug-level messages*/
#define LOG_LEVEL_MAX	LOG_LEVEL_DEBUG

// #if (LOG_COLORS == 1)
#if (1)
#define LOG_COLOR_RESET	 "\033[0m"
#define LOG_BOLD_TEXT	 "\033[1m"
#define LOG_BLACK_TEXT	 "\033[30;1m"
#define LOG_RED_TEXT	 "\033[31;1m"
#define LOG_GREEN_TEXT	 "\033[32;1m"
#define LOG_YELLOW_TEXT	 "\033[33;1m"
#define LOG_BLUE_TEXT	 "\033[34;1m"
#define LOG_MAGENTA_TEXT "\033[35;1m"
#define LOG_CYAN_TEXT	 "\033[36;1m"
#define LOG_WHITE_TEXT	 "\033[37;1m"
#endif

#ifndef LOG_LEVEL
/** Default maximum log level.
 *
 * This is the maximum log level that will be compiled in.
 * To set a custom log level, define the LOG_LEVEL before including this header
 * (e.g., as a compiler definition)
 */
#define LOG_LEVEL LOG_LEVEL_MAX
#endif

#if (LOG_LEVEL > LOG_LEVEL_OFF)
void platform_log_msg(const char *format, ...);
#endif

#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
#define log_error(FORMAT, ...) platform_log_msg(LOG_RED_TEXT "[%llu]E: " FORMAT "\r\n" LOG_COLOR_RESET, 0, ##__VA_ARGS__)
#else
#define log_error(FORMAT, ...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_WARN)
#define log_info(FORMAT, ...) platform_log_msg(LOG_YELLOW_TEXT "[%llu]W: " FORMAT "\r\n" LOG_COLOR_RESET, 0, ##__VA_ARGS__)
#else
#define log_info(FORMAT, ...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_INFO)
#define log_info(FORMAT, ...) platform_log_msg(COLOR_RESET "[%llu]I: " FORMAT "\r\n", 0, ##__VA_ARGS__)
#else
#define log_info(FORMAT, ...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
#define log_debug(FORMAT, ...) platform_log_msg(LOG_GREEN_TEXT "[%llu]D: " FORMAT "\r\n" LOG_COLOR_RESET, 0, ##__VA_ARGS__)
#else
#define log_debug(FORMAT, ...)
#endif
