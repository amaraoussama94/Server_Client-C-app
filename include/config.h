/**
 * @file config.h
 * @brief Configuration loader and parser for runtime settings (e.g., ports, paths, feature toggles).
 *        Supports environment overrides and default fallbacks.
 *        Extended to support client mode selection for multi-port routing.
 * @author Oussama Amara
 * @version 0.6
 * @date 2025-09-07
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Configuration structure holding runtime settings.
 */
typedef struct {
    char host[64];     ///< Server hostname or IP
    int port;          ///< Server port
    int enable_chat;   ///< Feature toggle for chat
    int enable_game;   ///< Feature toggle for game
    int enable_file;   ///< Feature toggle for file transfer
    char mode[32];     ///< Client mode: "msg", "file", "game"
} Config;

/**
 * @brief Loads configuration from file or environment.
 * @param[in] path Path to config file.
 * @param[out] cfg Pointer to Config struct to populate.
 * @return 0 on success, -1 on failure.
 */
int load_config(const char* path, Config* cfg);

/**
 * @brief Retrieves a configuration value by key.
 * @param[in] key Configuration key.
 * @return Value string, or NULL if not found.
 */
const char* get_config_value(const char* key);

#endif // CONFIG_H
