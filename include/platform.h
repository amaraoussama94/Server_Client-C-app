/**
 * @file platform.h
 * @brief Platform abstraction layer for OS-specific operations (e.g., path handling, socket setup, etc.). Ensures cross-platform compatibility across Linux, Windows, macOS.
 * @author Oussama Amara
 * @version 1.0
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

/**
 * @brief Prints the current working directory to the log.
 *        Useful for debugging relative path issues across platforms.
 */
void print_working_directory();

/**
 * @brief Returns the current working directory as an absolute path.
 *        Useful for resolving asset paths across platforms.
 * @return Pointer to static buffer containing the absolute path, or NULL on failure.
 */
const char* get_working_directory_path() ;

/**
 * @brief Resolves the absolute path to a file inside the assets folder,
 *        searching upward from the current directory.
 * @param subfolder Subdirectory inside assets (e.g., "to_send")
 * @param filename Name of the file to resolve
 * @return Pointer to static buffer containing the full path, or NULL on failure
 */
const char* resolve_asset_path(const char* subfolder, const char* filename) ;
#endif // PLATFORM_H
