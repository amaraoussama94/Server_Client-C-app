/**
 * @file server.c
 * @brief Implements the main server logic: multi-port socket setup, config loading, signal handling, and feature dispatch loop.
 *        Supports graceful shutdown via SIGINT and ensures port reuse with SO_REUSEADDR.
 *        Accepts incoming TCP connections on dedicated ports for chat, file, and game features.
 *        Routes each connection to the appropriate dispatcher module.
 * @author Oussama Amara
 * @version 0.7
 * @date 2025-09-07
 */

#include "server.h"
#include "connection.h"
#include "dispatcher.h"
#include "config.h"
#include "logger.h"
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

/*
 * Flag to control server loop and support graceful shutdown.
 */
volatile sig_atomic_t server_running = 1;


/**
 * @brief Signal handler for SIGINT (Ctrl+C).
 *        Sets shutdown flag and logs shutdown intent.
 * @param sig Signal number.
 */
void handle_sigint(int sig) {
    (void)sig;
    log_message(LOG_INFO, "Interrupt received. Shutting down server...");
    server_running = 0;
}

/**
 * @brief Runs the server application.
 *        Loads configuration, sets up sockets for each feature, and enters dispatch loop.
 * @param argc Argument count.
 * @param argv Argument vector. Expects argv[1] to be path to config file.
 * @return 0 on success, non-zero on failure.
 */
int run_server(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_path>\n", argv[0]);
        return 1;
    }

    log_message(LOG_INFO, "Attempting to load config from: %s", argv[1]);

    Config cfg;
    if (load_config(argv[1], &cfg) != 0) {
        fprintf(stderr, "[-] Failed to load server config.\n");
        return 1;
    }

    set_log_level(LOG_INFO);
    log_message(LOG_INFO, "Initializing multi-port server...");

    signal(SIGINT, handle_sigint); // Register shutdown handler

    win_socket_init();

    int ports[3] = { PORT_CHAT, PORT_FILE, PORT_GAME };
    int sockfds[3];

    struct sockaddr_in servaddr;

    /**
     * @brief Create and bind sockets for each feature port.
     */
    for (int i = 0; i < 3; ++i) {
        create_socket(&sockfds[i]);

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(ports[i]);

        socket_bind(&servaddr, &sockfds[i]);
        socket_listening(&sockfds[i]);

        log_message(LOG_INFO, "Listening on port %d", ports[i]);
    }

    /**
     * @brief Main server loop: accepts clients on each port, parses commands, and dispatches to correct feature.
     */
    while (server_running) {
        for (int i = 0; i < 3; ++i) {
            struct sockaddr_in cli;
            int len = sizeof(cli);
            int connfd = accept(sockfds[i], (struct sockaddr*)&cli, &len);
            if (connfd < 0) continue;

            char buffer[1024] = {0};
            int received = recv(connfd, buffer, sizeof(buffer) - 1, 0);
            if (received <= 0) {
                log_message(LOG_WARN, "Failed to receive data or client disconnected.");
                #ifdef _WIN32
                    closesocket(sockfds[i]);
                #else
                    close(sockfds[i]);
                #endif
                continue;
            }
            buffer[received] = '\0';

            char* ip = inet_ntoa(cli.sin_addr);
            log_message(LOG_INFO, "Received data on port %d from %s", ports[i], ip);

            ParsedCommand cmd;
            if (parse_command(buffer, &cmd) == 0) {
                if (ports[i] == PORT_CHAT) {
                    dispatch_chat_command(&cmd, connfd, cli);
                } else if (ports[i] == PORT_FILE) {
                    dispatch_file_command(&cmd, connfd, cli);
                } else if (ports[i] == PORT_GAME) {
                    dispatch_game_command(&cmd, connfd, cli);
                } else {
                    log_message(LOG_WARN, "Unknown port: %d", ports[i]);
                }
            }

        #ifdef _WIN32
            closesocket(connfd);
        #else
            close(connfd);
        #endif
        }
    }

    /**
     * @brief Cleanup: close all sockets and release resources.
     */
    for (int i = 0; i < 3; ++i) {
        #ifdef _WIN32
            closesocket(sockfds[i]);
        #else
            close(sockfds[i]);
        #endif
    }

    win_socket_cleanup();
    log_message(LOG_INFO, "Server shutdown complete. All ports released.");
    return 0;
}

/**
 * @brief Entry point for the server application.
 *        Delegates to run_server().
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code from run_server().
 */
int main(int argc, char** argv) {
    return run_server(argc, argv);
}
