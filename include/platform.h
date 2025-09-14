/**
 * @file platform.h
 * @brief Platform abstraction layer for OS-specific operations (e.g., path handling, socket setup, etc.). Ensures cross-platform compatibility across Linux, Windows, macOS.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
    #define PATH_SEPARATOR '\\'
#else
    #define PATH_SEPARATOR '/'
#endif
/**
 * @brief Sleeps for the specified number of milliseconds.
 * @param milliseconds Number of milliseconds to sleep.
 * @return void.
 */

void sleep_ms(int milliseconds);
/**
 * @brief Returns the platform-specific temporary directory path.
 * @return Path string.
 */
const char* get_temp_dir();

#endif // PLATFORM_H
