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
 * @brief Returns the platform-specific temporary directory path.
 * @return Path string.
 */
const char* get_temp_dir();

#endif // PLATFORM_H
