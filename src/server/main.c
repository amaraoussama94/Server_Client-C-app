/**
 * @file server.c
 * @brief Multi-threaded, multi-port server using select() for concurrent client acceptance.
 *        Supports chat, file, and game features with thread-per-client handling.
 *        Compatible with both Windows and Linux platforms.
 * @author Oussama
 * @version 3.0
 * @date 2025-09-16
 */

#include "server.h"
#include "connection.h"
#include "dispatcher.h"
#include "config.h"
#include "logger.h"
#include "protocol.h"
#include "client_registry.h"
#include "platform.h"
#include "platform_thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <winsock2.h>
#define socklen_t int
#else
#include <unistd.h>
#include <sys/select.h>
#endif

volatile sig_atomic_t server_running = 1;

/**
 * @brief Signal handler for graceful shutdown.
 */
void handle_sigint(int sig) {
    (void)sig;
    log_message(LOG_INFO, "Interrupt received. Shutting down server...");
    server_running = 0;
}

/**
 * @brief Thread function to handle individual client connection.
 * @param arg Pointer to ClientArgs containing socket and client info.
 * @return THREAD_FUNC return value (platform-specific).
 */
THREAD_FUNC handle_client(void* arg) {
    ClientArgs* args = (ClientArgs*)arg;
    int connfd = args->connfd;
    struct sockaddr_in cli = args->cli;
    free(arg);

    int client_id = register_client(connfd, cli);
    if (client_id < 0) {
        log_message(LOG_ERROR, "Max clients reached.");
#ifdef _WIN32
        closesocket(connfd);
#else
        close(connfd);
#endif
        THREAD_RETURN;
    }

    char buffer[MAX_COMMAND_LENGTH];
    build_frame("system", 0, client_id, "ID_ASSIGN", "READY", buffer);
    send(connfd, buffer, strlen(buffer), 0);
    log_message(LOG_INFO, "Sent ID_ASSIGN to client %d", client_id);

    for (int j = 0; j < MAX_CLIENTS; ++j) {
        if (j + 1 != client_id && get_socket_by_id(j + 1) > 0) {
            char list_msg[MAX_COMMAND_LENGTH];
            snprintf(list_msg, sizeof(list_msg), "%d,%s", j + 1, "Client");
            build_frame("system", 0, client_id, list_msg, "LIST", buffer);
            send(connfd, buffer, strlen(buffer), 0);
        }
    }

    build_frame("system", 0, client_id, "You may begin", "START", buffer);
    send(connfd, buffer, strlen(buffer), 0);

    int received = recv(connfd, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        log_message(LOG_INFO, "Received raw frame from client %d: %s", client_id, buffer);

        ParsedCommand cmd;
        if (parse_command(buffer, &cmd) == 0) {
            log_message(LOG_INFO, "Parsed frame → channel=%s src=%d dest=%d status=%s msg=%s",
                        cmd.channel, cmd.src_id, cmd.dest_id, cmd.status, cmd.message);
            dispatch_command(&cmd);
        } else {
            log_message(LOG_WARN, "Failed to parse frame from client %d", client_id);
        }
    }

#ifdef _WIN32
    closesocket(connfd);
#else
    close(connfd);
#endif
    unregister_client(client_id);
    log_message(LOG_INFO, "Client %d disconnected.", client_id);
    THREAD_RETURN;
}

/**
 * @brief Main server entry point. Loads config, sets up sockets, and uses select() to monitor all ports.
 *        Spawns threads for each accepted client.
 */
int run_server(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_path>\n", argv[0]);
        return 1;
    }

    Config cfg;
    if (load_config(argv[1], &cfg) != 0) {
        fprintf(stderr, "[-] Failed to load server config.\n");
        return 1;
    }

    set_log_level(LOG_INFO);
    signal(SIGINT, handle_sigint);
    win_socket_init();
    init_registry();

    int ports[3] = { cfg.port_chat, cfg.port_file, cfg.port_game };
    int sockfds[3];

    for (int i = 0; i < 3; ++i) {
        create_socket(&sockfds[i]);

        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(cfg.host);
        servaddr.sin_port = htons(ports[i]);

        socket_bind(&servaddr, &sockfds[i]);
        socket_listening(&sockfds[i]);

        log_message(LOG_INFO, "Listening on %s:%d", cfg.host, ports[i]);
    }

    while (server_running) {
        // Use select() to monitor multiple sockets
        fd_set readfds;
        FD_ZERO(&readfds);
        int maxfd = -1;

        // Add all listening sockets to the readfds set for select() monitoring.
        // Also track the highest socket descriptor for select()'s maxfd parameter.
        for (int i = 0; i < 3; ++i) {
            FD_SET(sockfds[i], &readfds);
            if (sockfds[i] > maxfd) maxfd = sockfds[i];
        }

        struct timeval timeout = { .tv_sec = 1, .tv_usec = 0 };
        int ready = select(maxfd + 1, &readfds, NULL, NULL, &timeout);

        if (ready > 0) {
            for (int i = 0; i < 3; ++i) {
                // Check if this socket is ready to accept a new connection
                if (FD_ISSET(sockfds[i], &readfds)) {
                    struct sockaddr_in cli;
                    socklen_t len = sizeof(cli);
                    int connfd = accept(sockfds[i], (struct sockaddr*)&cli, &len);
                    if (connfd < 0) continue;

                    log_message(LOG_INFO, "Accepted connection on port %d from %s:%d",
                                ports[i], inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

                    ClientArgs* args = malloc(sizeof(ClientArgs));
                    args->connfd = connfd;
                    args->cli = cli;
                    args->port = ports[i];

                    thread_t tid;
                    if (create_thread(&tid, handle_client, args) == 0) {
                        detach_thread(tid);
                    } else {
                        log_message(LOG_ERROR, "Failed to create thread.");
                        free(args);
                    }
                }
            }
        }

        check_timeouts(30);
        if (!has_active_clients()) {
            log_message(LOG_INFO, "No active clients. Sleeping...");
            sleep_ms(5000);
        }
    }

    for (int i = 0; i < 3; ++i) {
#ifdef _WIN32
        closesocket(sockfds[i]);
#else
        close(sockfds[i]);
#endif
    }

    win_socket_cleanup();
    log_message(LOG_INFO, "Server shutdown complete.");
    return 0;
}

/**
 * @brief Main function wrapper.
 */
int main(int argc, char** argv) {
    return run_server(argc, argv);
}
