/**
 * @file logger.c
 * @brief Logging system with configurable levels and output targets.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-08-15
 */

#include "logger.h"
#include <stdio.h>
#include <stdarg.h>

LogLevel current_level = LOG_INFO;

void set_log_level(LogLevel level) {
    current_level = level;
}

void log_message(LogLevel level, const char* format, ...) {
    if (level < current_level) return;

    const char* level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};
    fprintf(stderr, "[%s] ", level_str[level]);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}
