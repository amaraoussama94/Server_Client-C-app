/**
 * @file platform.c
 * @brief Cross-platform compatibility utilities.
 *       Provides functions for sleep and temporary directory retrieval.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-08-15
 */

#include "platform.h"
#include "logger.h"
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

void sleep_ms(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}
const char* get_temp_dir() {
#ifdef _WIN32
    return getenv("TEMP") ? getenv("TEMP") : "C:\\Temp";
#else
    return "/tmp";
#endif

}

/**
 * @brief Prints the current working directory to the log.
 *        Useful for debugging relative path issues across platforms.
 */
void print_working_directory() {
    char cwd[512];

#ifdef _WIN32
    if (_getcwd(cwd, sizeof(cwd)) != NULL)
#else
    if (getcwd(cwd, sizeof(cwd)) != NULL)
#endif
    {
        log_message(LOG_INFO, "[PLATFORM] Current working directory: %s", cwd);
    } else {
        log_message(LOG_ERROR, "[PLATFORM] Failed to retrieve working directory.");
    }
}
