/**
 * @file logger.h
 * @brief Logging utilities with configurable verbosity and output targets (console, file, etc.). Supports cross-platform formatting and timestamps.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

/**
 * @brief Initializes the logger with a given verbosity level.
 * @param[in] level Minimum log level to display.
 */
void set_log_level(LogLevel level);

/**
 * @brief Logs a formatted message at the specified level.
 * @param[in] level Log severity level.
 * @param[in] fmt Format string (printf-style).
 * @param[in] ... Variable arguments.
 */
void log_message(LogLevel level, const char* fmt, ...);

#endif // LOGGER_H
