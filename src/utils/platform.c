/**
 * @file platform.c
 * @brief Cross-platform compatibility utilities.
 *       Provides functions for sleep and temporary directory retrieval.
 * @author Oussama Amara
 * @version 1.1
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>

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

const char* get_working_directory_path() {
    static char cwd[512];

#ifdef _WIN32
    if (_getcwd(cwd, sizeof(cwd)) != NULL)
#else
    if (getcwd(cwd, sizeof(cwd)) != NULL)
#endif
    {
        return cwd;
    } else {
        log_message(LOG_ERROR, "[PLATFORM] Failed to retrieve working directory.");
        return NULL;
    }
}

const char* resolve_asset_path(const char* subfolder, const char* filename) {
    static char full_path[PATH_MAX];
    char cwd[PATH_MAX];

    if (!getcwd(cwd, sizeof(cwd))) {
        log_message(LOG_ERROR, "[PATH] Failed to get current working directory.");
        return NULL;
    }

    while (1) {
        char test_path[PATH_MAX];
        snprintf(test_path, sizeof(test_path), "%s/assets", cwd);

        struct stat st;
        if (stat(test_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            snprintf(full_path, sizeof(full_path), "%s/assets/%s/%s", cwd, subfolder, filename);
            return full_path;
        }

        // Move up one directory
        char* last_sep = strrchr(cwd, '/');
#ifdef _WIN32
        if (!last_sep) last_sep = strrchr(cwd, '\\');
#endif
        if (!last_sep) break;

        *last_sep = '\0';  // Trim the last segment
    }

    log_message(LOG_ERROR, "[PATH] Could not locate assets folder.");
    return NULL;
}

