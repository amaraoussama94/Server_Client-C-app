/**
 * @file config.c
 * @brief Loads and parses configuration files.
 */

#include "config.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int load_config(const char* path, Config* cfg) {
    FILE* file = fopen(path, "r");
    if (!file || !cfg) {
        log_message(LOG_ERROR, "Failed to open config file: %s", path);
        return 0;
    }

    char key[64], value[256];
    while (fscanf(file, "%s %s", key, value) == 2) {
        if (strcmp(key, "host") == 0) {
            strncpy(cfg->host, value, sizeof(cfg->host) - 1);
            cfg->host[sizeof(cfg->host) - 1] = '\0';
        } else if (strcmp(key, "port") == 0) {
            cfg->port = atoi(value);
        }
    }

    fclose(file);
    log_message(LOG_INFO, "Config loaded: %s:%d", cfg->host, cfg->port);
    return 1;
}
