/**
 * @file server.c
 * @brief Multi-threaded server implementation supporting chat, file, and game features.
 *        Accepts clients on multiple ports and spawns a dedicated thread per connection.
 *        Uses cross-platform thread abstraction for compatibility with Windows and Linux.
 * @author Oussama
 * @version 2.0
 * @date 2025-09-14
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
typedef int socklen_t; 
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif

volatile sig_atomic_t server_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    log_message(LOG_INFO, "Interrupt received. Shutting down server...");
    server_running = 0;
}

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
    // work around to avoid message clumping 
    // to be improved with proper message framing
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
