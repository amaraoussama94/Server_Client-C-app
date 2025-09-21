/**
 * @file server.c
 * @brief Entry point and orchestration logic for the server application.
 *        Loads config, sets up sockets, launches thread-per-client and background sync.
 *        Uses select() for multi-port monitoring and supports chat, file, and game features.
 * @date 2025-09-21
 * @author Oussama
 * @version 3.1
 */

#include "server.h"
#include "connection.h"
#include "config.h"
#include "logger.h"
#include "client_registry.h"
#include "platform.h"
#include "platform_thread.h"
#include "thread_logic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#define socklen_t int
#else
#include <unistd.h>
#include <sys/select.h>
#endif
#include <signal.h>

/**
 * Time of inactive client after which we kill the client
 */
#define CLIENT_TIME_OUT 300 // seconds

/**
 * Server idle sleep duration when no clients are connected
 */
#define SERVER_IDLE_SLEEP_MS 5000 // milliseconds
/**
 * @brief Global flag to control server running state.
 */
volatile sig_atomic_t server_running = 1;

/**
 * @brief Signal handler for graceful shutdown.
 * @param sig Signal number (unused).
 */
void handle_sigint(int sig) {
    (void)sig;
    log_message(LOG_INFO, "Interrupt received. Shutting down server...");
    server_running = 0;
}

/**
 * @brief Main server entry point. Loads config, sets up sockets, and uses select() to monitor all ports.
 *        Spawns threads for each accepted client and launches background sync thread.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 on success, non-zero on error.
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

    // Launch background sync thread
    thread_t sync_thread;
    create_thread(&sync_thread, broadcast_client_list, NULL);
    detach_thread(sync_thread);

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
        fd_set readfds;
        FD_ZERO(&readfds);
        int maxfd = -1;

        for (int i = 0; i < 3; ++i) {
            FD_SET(sockfds[i], &readfds);
            if (sockfds[i] > maxfd) maxfd = sockfds[i];
        }

        struct timeval timeout = { .tv_sec = 1, .tv_usec = 0 };
        int ready = select(maxfd + 1, &readfds, NULL, NULL, &timeout);

        if (ready > 0) {
            for (int i = 0; i < 3; ++i) {
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
                    if (create_thread(&tid, handle_client_thread, args) == 0) {
                        detach_thread(tid);
                    } else {
                        log_message(LOG_ERROR, "Failed to create thread.");
                        free(args);
                    }
                }
            }
        }
        // Periodically check for client timeouts and clean up
        check_timeouts(CLIENT_TIME_OUT);
        if (!has_active_clients()) {
            log_message(LOG_INFO, "No active clients. Sleeping...");
            sleep_ms(SERVER_IDLE_SLEEP_MS);
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
