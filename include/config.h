/**
 * @file config.h
 * @brief Configuration structure for client and server applications.
 *        Server uses multi-port routing; client uses single-port feature selection.
 * @author Oussama Amara
 * @version 0.9
 * @date 2025-09-07
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Represents configuration loaded from file or environment.
 */
typedef struct {
    char host[64];       ///< Server IP or hostname
    int port;            ///< Client target port (used to select feature)
    int port_chat;       ///< Server port for chat service
    int port_file;       ///< Server port for file service
    int port_game;       ///< Server port for game service
} Config;

int load_config(const char* path, Config* cfg);
const char* get_config_value(const char* key);

#endif // CONFIG_H
