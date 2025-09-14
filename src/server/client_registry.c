/**
 * @file client_registry.c
 * @brief Implements client registration and activity tracking.
 *     Supports up to MAX_CLIENTS simultaneous clients.
 *    Tracks last activity for timeout handling.
 *  @date 2025-09-14
 * @author Oussama Amara
 * @version 0.1
 */

#include "client_registry.h"
#include "logger.h"
#include <string.h>
#include <unistd.h>

static ClientInfo clients[MAX_CLIENTS];

void init_registry() {
    memset(clients, 0, sizeof(clients));
    // init all IDs to -1 (indicating free slots)
    for (int i = 0; i < MAX_CLIENTS; ++i) clients[i].id = -1;
}

int register_client(int socket, struct sockaddr_in addr) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].id == -1) {
            clients[i].id = i + 1;// IDs start from 1 0 is reserved for server
            clients[i].socket = socket;
            clients[i].addr = addr;
            clients[i].last_activity = time(NULL);
            clients[i].active = 1;
            snprintf(clients[i].name, sizeof(clients[i].name), "Client%d", clients[i].id);
            return clients[i].id;
        }
    }
    return -1;
}

int get_socket_by_id(int id) {
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i].id == id && clients[i].active)
            return clients[i].socket;
    return -1;
}

int get_id_by_socket(int socket) {
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i].socket == socket && clients[i].active)
            return clients[i].id;
    return -1;
}

void update_activity(int id) {
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i].id == id)
            clients[i].last_activity = time(NULL);
}

void unregister_client(int id) {
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i].id == id) {
            close(clients[i].socket);
            clients[i].id = -1;
            clients[i].active = 0;
        }
}

void check_timeouts(int timeout_seconds) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active && (now - clients[i].last_activity) > timeout_seconds) {
            log_message(LOG_INFO, "Client %d timed out.", clients[i].id);
            unregister_client(clients[i].id);
        }
    }
}

int has_active_clients() {
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i].active)
            return 1;
    return 0;
}
