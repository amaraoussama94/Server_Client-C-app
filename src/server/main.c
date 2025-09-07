/**
 * @file server.c
 * @brief Implements the main server logic: socket setup, config loading, signal handling, and client dispatch loop.
 *        Supports graceful shutdown via SIGINT and ensures port reuse with SO_REUSEADDR.
 *        Accepts incoming TCP connections, parses commands, and dispatches to feature modules.
 * @author Oussama Amara
 * @version 0.6
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

/// Flag to control server loop and support graceful shutdown.
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
 *        Loads configuration, sets up socket, and enters client dispatch loop.
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
    log_message(LOG_INFO, "Starting server on %s:%d", cfg.host, cfg.port);

    signal(SIGINT, handle_sigint); // Register shutdown handler

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    int len = sizeof(cli);

    win_socket_init();
    create_socket(&sockfd); // Includes SO_REUSEADDR

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(cfg.port);

    socket_bind(&servaddr, &sockfd);
    socket_listening(&sockfd);

    /**
     * @brief Main server loop: accepts clients, receives messages, and dispatches commands.
     *        Loop exits cleanly when SIGINT is received.
     */
    while (server_running) {
        socket_accept(&connfd, &sockfd, &cli, &len);

        char buffer[1024] = {0};
        int received = recv(connfd, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) {
            log_message(LOG_WARN, "Failed to receive data or client disconnected.");
        #ifdef _WIN32
            closesocket(connfd);
        #else
            close(connfd);
        #endif
            continue;
        }
        buffer[received] = '\0';

        char* ip = inet_ntoa(cli.sin_addr);
        log_message(LOG_INFO, "Received data from %s", ip);

        ParsedCommand cmd;
        if (parse_command(buffer, &cmd) == 0) {
            dispatch_command(&cmd, connfd, cli);
        }

    #ifdef _WIN32
        closesocket(connfd);
    #else
        close(connfd);
    #endif
    }

#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif

    win_socket_cleanup();
    log_message(LOG_INFO, "Server shutdown complete. Socket released.");
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
