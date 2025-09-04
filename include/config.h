/**
 * @file config.h
 * @brief Configuration loader and parser for runtime settings (e.g., ports, paths, feature toggles).
 *  Supports environment overrides and default fallbacks.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Loads configuration from file or environment.
 * @param[in] path Path to config file.
 * @return 0 on success, -1 on failure.
 */
int load_config(const char* path);

/**
 * @brief Retrieves a configuration value by key.
 * @param[in] key Configuration key.
 * @return Value string, or NULL if not found.
 */
const char* get_config_value(const char* key);

#endif // CONFIG_H
