/**
 * @file config.c
 * @brief Loads and parses configuration files with support for feature toggles and environment overrides.
 *        Applies default values, then overrides from file and environment variables.
 *        Extended to support client mode selection for multi-port routing.
 * @author Oussama Amara
 * @version 0.6
 * @date 2025-09-07
 */

#include "config.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Internal helper to parse boolean-like values.
 * @param value String value to interpret.
 * @return 1 if value is "1" or "true" (case-insensitive), 0 otherwise.
 */
static int parse_bool(const char* value) {
    return (strcmp(value, "1") == 0 || strcasecmp(value, "true") == 0);
}

/**
 * @brief Loads configuration from file and applies environment overrides.
 *        Applies default values first, then overrides from file and environment.
 * @param path Path to configuration file.
 * @param cfg Pointer to Config struct to populate.
 * @return 0 on success, -1 on failure.
 */
int load_config(const char* path, Config* cfg) {
    // Apply default values
    strncpy(cfg->host, "127.0.0.1", sizeof(cfg->host) - 1);
    cfg->host[sizeof(cfg->host) - 1] = '\0';
    cfg->port = 8080;
    cfg->enable_chat = 1;
    cfg->enable_game = 0;
    cfg->enable_file = 1;
    strncpy(cfg->mode, "msg", sizeof(cfg->mode) - 1);
    cfg->mode[sizeof(cfg->mode) - 1] = '\0';

    // Attempt to open config file
    FILE* file = fopen(path, "r");
    if (!file) {
        log_message(LOG_ERROR, "Failed to open config file: %s", path);
        return -1;
    }

    // Parse key-value pairs from file
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[64], value[128];
        if (line[0] == '#' || strlen(line) < 3) continue;
        if (sscanf(line, "%63s %127s", key, value) == 2) {
            if (strcmp(key, "host") == 0) {
                strncpy(cfg->host, value, sizeof(cfg->host) - 1);
                cfg->host[sizeof(cfg->host) - 1] = '\0';
            } else if (strcmp(key, "port") == 0) {
                cfg->port = atoi(value);
            } else if (strcmp(key, "enable_chat") == 0) {
                cfg->enable_chat = parse_bool(value);
            } else if (strcmp(key, "enable_game") == 0) {
                cfg->enable_game = parse_bool(value);
            } else if (strcmp(key, "enable_file") == 0) {
                cfg->enable_file = parse_bool(value);
            } else if (strcmp(key, "mode") == 0) {
                strncpy(cfg->mode, value, sizeof(cfg->mode) - 1);
                cfg->mode[sizeof(cfg->mode) - 1] = '\0';
            }
        }
    }
    fclose(file);

    // Apply environment overrides
    const char* env_host = getenv("CONFIG_HOST");
    if (env_host) {
        strncpy(cfg->host, env_host, sizeof(cfg->host) - 1);
        cfg->host[sizeof(cfg->host) - 1] = '\0';
        log_message(LOG_INFO, "Overriding host from environment: %s", env_host);
    }

    const char* env_port = getenv("CONFIG_PORT");
    if (env_port) {
        cfg->port = atoi(env_port);
        log_message(LOG_INFO, "Overriding port from environment: %s", env_port);
    }

    const char* env_chat = getenv("CONFIG_ENABLE_CHAT");
    if (env_chat) {
        cfg->enable_chat = parse_bool(env_chat);
        log_message(LOG_INFO, "Overriding chat toggle from environment: %s", env_chat);
    }

    const char* env_game = getenv("CONFIG_ENABLE_GAME");
    if (env_game) {
        cfg->enable_game = parse_bool(env_game);
        log_message(LOG_INFO, "Overriding game toggle from environment: %s", env_game);
    }

    const char* env_file = getenv("CONFIG_ENABLE_FILE");
    if (env_file) {
        cfg->enable_file = parse_bool(env_file);
        log_message(LOG_INFO, "Overriding file toggle from environment: %s", env_file);
    }

    const char* env_mode = getenv("CONFIG_MODE");
    if (env_mode) {
        strncpy(cfg->mode, env_mode, sizeof(cfg->mode) - 1);
        cfg->mode[sizeof(cfg->mode) - 1] = '\0';
        log_message(LOG_INFO, "Overriding mode from environment: %s", env_mode);
    }

    // Final configuration summary
    log_message(LOG_INFO, "Config loaded: %s:%d (chat=%d, game=%d, file=%d, mode=%s)",
                cfg->host, cfg->port, cfg->enable_chat, cfg->enable_game, cfg->enable_file, cfg->mode);

    return 0;
}

/**
 * @brief Retrieves a configuration value from the environment.
 * @param key Environment variable name.
 * @return Value string or NULL if not set.
 */
const char* get_config_value(const char* key) {
    return getenv(key);
}
