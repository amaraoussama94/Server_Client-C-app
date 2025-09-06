/**
 * @file config.c
 * @brief Loads and parses configuration files with support for feature toggles and environment overrides.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#include "config.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Internal helper to parse boolean-like values.
 */
static int parse_bool(const char* value) {
    return (strcmp(value, "1") == 0 || strcasecmp(value, "true") == 0);
}

int load_config(const char* path, Config* cfg) {
    FILE* file = fopen(path, "r");
    if (!file) {
        log_message(LOG_ERROR, "Failed to open config file: %s", path);
        return -1;
    }

    // Set defaults
    strcpy(cfg->host, "127.0.0.1");
    cfg->port = 8080;
    cfg->enable_chat = 1;
    cfg->enable_game = 0;
    cfg->enable_file = 1;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[64], value[128];
        if (sscanf(line, "%63s %127s", key, value) == 2) {
            if (strcmp(key, "host") == 0) {
                strncpy(cfg->host, value, sizeof(cfg->host) - 1);
            } else if (strcmp(key, "port") == 0) {
                cfg->port = atoi(value);
            } else if (strcmp(key, "enable_chat") == 0) {
                cfg->enable_chat = parse_bool(value);
            } else if (strcmp(key, "enable_game") == 0) {
                cfg->enable_game = parse_bool(value);
            } else if (strcmp(key, "enable_file") == 0) {
                cfg->enable_file = parse_bool(value);
            }
        }
    }

    fclose(file);
    log_message(LOG_INFO, "Config loaded: %s:%d (chat=%d, game=%d, file=%d)",
                cfg->host, cfg->port, cfg->enable_chat, cfg->enable_game, cfg->enable_file);
    return 0;
}

const char* get_config_value(const char* key) {
    return getenv(key); // Optional: extend to search loaded config
}
