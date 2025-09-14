/**
 * @file server.c
 * @brief Main server loop: socket setup, client registry, protocol dispatch, and timeout handling.
 *        Supports multi-port feature routing and delivery confirmation.
 *       Graceful shutdown via SIGINT.
 * @date 2025-09-14
 * @author Oussama Amara
 * @version 1.2
 */

#include "server.h"
#include "connection.h"
#include "dispatcher.h"
#include "config.h"
#include "logger.h"
#include "protocol.h"
#include "client_registry.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

volatile sig_atomic_t server_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    log_message(LOG_INFO, "Interrupt received. Shutting down server...");
    server_running = 0;
}

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
        for (int i = 0; i < 3; ++i) {
            struct sockaddr_in cli;
            int len = sizeof(cli);
            int connfd = accept(sockfds[i], (struct sockaddr*)&cli, &len);
            log_message(LOG_INFO, "Accepted connection on port %d from %s:%d",
            ports[i], inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

            if (connfd < 0) continue;

            int client_id = register_client(connfd, cli);
            if (client_id < 0) {
                log_message(LOG_ERROR, "Max clients reached.");
            #ifdef _WIN32
                closesocket(connfd);
            #else
                close(connfd);
            #endif
                continue;
            }

            char welcome[MAX_COMMAND_LENGTH];
            build_frame("system", 0, client_id, "ID_ASSIGN", "READY", welcome);
            log_message(LOG_INFO, "Sending ID assignment to client %d: %s", client_id, welcome);
            send(connfd, welcome, strlen(welcome), 0);

            for (int j = 0; j < MAX_CLIENTS; ++j) {
                if (j + 1 != client_id && get_socket_by_id(j + 1) > 0) {
                    char list_msg[MAX_COMMAND_LENGTH];
                    //prepare client list message
                    snprintf(list_msg, sizeof(list_msg), "%d,%s", j + 1, "Client");
                    build_frame("system", 0, client_id, list_msg, "LIST", welcome);
                    send(connfd, welcome, strlen(welcome), 0);
                }
            }
            char buffer[MAX_COMMAND_LENGTH] = {0};
            //inform the client that he can start sending
            build_frame("system", 0, client_id, "You may begin", "START", buffer);
            send(connfd, buffer, strlen(buffer), 0);
            log_message(LOG_INFO, "Client %d registered.", client_id);

            int received = recv(connfd, buffer, sizeof(buffer) - 1, 0);
            log_message(LOG_INFO, "Received raw frame from client %d: %s", client_id, buffer);
            if (received <= 0) {
                log_message(LOG_WARN, "Client disconnected or failed to send.");
                unregister_client(client_id);
                continue;
            }
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


        #ifdef _WIN32
            closesocket(connfd);
        #else
            close(connfd);
        #endif;
        }
        //we wait 30 seconds before kicking out inactive clients
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
    #endif;
    }

    win_socket_cleanup();
    log_message(LOG_INFO, "Server shutdown complete.");
    return 0;
}

int main(int argc, char** argv) {
    return run_server(argc, argv);
}
