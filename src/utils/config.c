/**
 * @file config.c
 * @brief Loads and parses configuration files.
 */

#include "config.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>

int load_config(const char* path, Config* cfg) {
    FILE* file = fopen(path, "r");
    if (!file) {
        log_message(LOG_ERROR, "Failed to open config file: %s", path);
        return 0;
    }

    fscanf(file, "%s %d", cfg->host, &cfg->port);
    fclose(file);
    log_message(LOG_INFO, "Config loaded: %s:%d", cfg->host, cfg->port);
    return 1;
}
