/**
 * @file platform.c
 * @brief Cross-platform compatibility utilities.
 *       Provides functions for sleep and temporary directory retrieval.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-08-15
 */

#include "platform.h"

#ifdef _WIN32
#include <windows.h>
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

