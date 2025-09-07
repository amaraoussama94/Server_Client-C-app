/**
 * @file config.c
 * @brief Loads and parses configuration files with support for multi-port routing and environment overrides.
 *        Applies default values, then overrides from file and environment variables.
 *        Used by both server and client to configure host and ports.
 * @author Oussama Amara
 * @version 0.9
 * @date 2025-09-07
 */
/**
 * To do list:
 * -Add  parsing config file with support for multi-port routing and environment overrides.
 * -Add default values for host and ports.
 */
#include "config.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int load_config(const char* path, Config* cfg) {
    /**
     * load  default values for host and ports
     */
    strncpy(cfg->host, "127.0.0.1", sizeof(cfg->host) - 1);
    cfg->host[sizeof(cfg->host) - 1] = '\0';

    cfg->port = 8081;        // Default client port (chat)
    cfg->port_chat = 8081;   // Default server ports
    cfg->port_file = 8082;
    cfg->port_game = 8083;
    /**
     *  ovveride default values with config file if it exists
     */
    FILE* file = fopen(path, "r");
    if (!file) {
        log_message(LOG_ERROR, "Failed to open config file: %s", path);
        return -1;
    }

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
            } else if (strcmp(key, "port_chat") == 0) {
                cfg->port_chat = atoi(value);
            } else if (strcmp(key, "port_file") == 0) {
                cfg->port_file = atoi(value);
            } else if (strcmp(key, "port_game") == 0) {
                cfg->port_game = atoi(value);
            }
        }
    }
    fclose(file);

    // Environment overrides
    const char* env_host = getenv("CONFIG_HOST");
    if (env_host) {
        strncpy(cfg->host, env_host, sizeof(cfg->host) - 1);
        cfg->host[sizeof(cfg->host) - 1] = '\0';
        log_message(LOG_INFO, "Overriding host from environment: %s", env_host);
    }

    const char* env_port = getenv("CONFIG_PORT");
    if (env_port) {
        cfg->port = atoi(env_port);
        log_message(LOG_INFO, "Overriding client port from environment: %s", env_port);
    }

    const char* env_chat = getenv("CONFIG_PORT_CHAT");
    if (env_chat) {
        cfg->port_chat = atoi(env_chat);
        log_message(LOG_INFO, "Overriding chat port from environment: %s", env_chat);
    }

    const char* env_file = getenv("CONFIG_PORT_FILE");
    if (env_file) {
        cfg->port_file = atoi(env_file);
        log_message(LOG_INFO, "Overriding file port from environment: %s", env_file);
    }

    const char* env_game = getenv("CONFIG_PORT_GAME");
    if (env_game) {
        cfg->port_game = atoi(env_game);
        log_message(LOG_INFO, "Overriding game port from environment: %s", env_game);
    }

    log_message(LOG_INFO, "Config loaded: host=%s, port=%d (chat=%d, file=%d, game=%d)",
                cfg->host, cfg->port, cfg->port_chat, cfg->port_file, cfg->port_game);

    return 0;
}

const char* get_config_value(const char* key) {
    return getenv(key);
}
