/**
 * @file thread_logic.c
 * @brief Implements server-side thread logic for client handling and synchronization.
 *        Includes per-client thread and background broadcaster.
 * @date 2025-09-21
 * @author Oussama
 * @version 1.0
 */

#include "thread_logic.h"
#include "client_registry.h"
#include "protocol.h"
#include "dispatcher.h"
#include "logger.h"
#include "server.h"
#include "platform.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

extern volatile sig_atomic_t server_running;

THREAD_FUNC handle_client_thread(void* arg) {
    ClientArgs* args = (ClientArgs*)arg;
    int connfd = args->connfd;
    struct sockaddr_in cli = args->cli;
    free(arg);

    int client_id = register_client(connfd, cli);
    if (client_id < 0) {
        log_message(LOG_ERROR, "Max clients reached.");
        close(connfd);
        THREAD_RETURN;
    }

    char buffer[MAX_COMMAND_LENGTH];
    build_frame("system", 0, client_id, "ID_ASSIGN", "READY", buffer);
    send(connfd, buffer, strlen(buffer), 0);
    log_message(LOG_INFO, "Sent ID_ASSIGN to client %d", client_id);

    while (1) {
        int received = recv(connfd, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) break;

        buffer[received] = '\0';
        ParsedCommand cmd;
        if (parse_command(buffer, &cmd) == 0) {
            dispatch_command(&cmd);
        } else {
            log_message(LOG_WARN, "Failed to parse frame from client %d", client_id);
        }
    }

    close(connfd);
    unregister_client(client_id);
    log_message(LOG_INFO, "Client %d disconnected.", client_id);
    THREAD_RETURN;
}

THREAD_FUNC broadcast_client_list(void* arg) {
    (void)arg;
    char buffer[MAX_COMMAND_LENGTH];

    while (server_running) {
        int active_count = 0;
        for (int i = 0; i < MAX_CLIENTS; ++i)
            if (get_socket_by_id(i + 1) > 0)
                active_count++;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int sock_i = get_socket_by_id(i + 1);
            if (sock_i <= 0) continue;

            for (int j = 0; j < MAX_CLIENTS; ++j) {
                if (j + 1 == i + 1) continue;
                int sock_j = get_socket_by_id(j + 1);
                if (sock_j > 0) {
                    char list_msg[MAX_COMMAND_LENGTH];
                    snprintf(list_msg, sizeof(list_msg), "%d,%s", j + 1, "Client");
                    build_frame("system", 0, i + 1, list_msg, "LIST", buffer);
                    send(sock_i, buffer, strlen(buffer), 0);
                }
            }

            if (active_count >= 2) {
                build_frame("system", 0, i + 1, "You may begin", "START", buffer);
            } else {
                build_frame("system", 0, i + 1, "Waiting for another client...", "WAIT", buffer);
            }
            send(sock_i, buffer, strlen(buffer), 0);
        }

        sleep_ms(3000);
    }

    THREAD_RETURN;
}
