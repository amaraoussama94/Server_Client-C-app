/**
 * @file server.c
 * @brief Implements the main server logic: socket setup, config loading, and client handling loop.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */
/**
 * @file main.c
 * @brief Entry point for the server application.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
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

int run_server(int argc, char** argv) {
    Config cfg;
    if (!load_config(argv[1], &cfg)) {
        fprintf(stderr, "[-] Failed to load server config.\n");
        return 1;
    }

    set_log_level(LOG_INFO);
    log_message(LOG_INFO, "Starting server on %s:%d", cfg.host, cfg.port);

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    int len = sizeof(cli);

    win_socket_init();
    create_socket(&sockfd);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(cfg.port);

    socket_bind(&servaddr, &sockfd);
    socket_listening(&sockfd);

    while (1) {
        socket_accept(&connfd, &sockfd, &cli, &len);

        char buffer[1024] = {0};
        recv(connfd, buffer, sizeof(buffer), 0);

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

    win_socket_cleanup();
    return 0;
}

int main(int argc, char** argv) {
    return run_server(argc, argv);
}
