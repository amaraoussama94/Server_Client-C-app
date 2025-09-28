/**
 * @file client_main.c
 * @brief Main client logic: config loading, connection setup, handshake, and user input.
 *        Launches listener thread and sends chat/file/game messages.
 *        Uses custom protocol format: <CRC>|<CHANNEL>|<SRC_ID>|<DEST_ID>|<MESSAGE>|<STATUS>
 *        Supports chat, file, and game features based on port configuration.
 *        Real-time reception is handled by a background listener thread.
 * @author Oussama Amara
 * @version 1.5
 * @date 2025-09-28
 */

#include "client.h"
#include "config.h"
#include "logger.h"
#include "protocol.h"
#include "chat.h"
#include "client_listener.h"
#include "platform_thread.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile int client_running = 1;

/**
 * @brief Entry point for client logic.
 *        Loads config, connects to server, starts listener thread, and handles user input.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code.
 */
int run_client(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_path>\n", argv[0]);
        return 1;
    }

    Config cfg;
    if (load_config(argv[1], &cfg) != 0) {
        fprintf(stderr, "[-] Failed to load config.\n");
        return 1;
    }

    set_log_level(LOG_INFO);

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "[-] WSAStartup failed.\n");
        return 1;
    }
#endif

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (sockfd == INVALID_SOCKET) {
        fprintf(stderr, "[-] Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
#else
    if (sockfd < 0) {
        perror("[-] Socket creation failed");
        return 1;
    }
#endif

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(cfg.port);
    servaddr.sin_addr.s_addr = inet_addr(cfg.host);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
#ifdef _WIN32
        fprintf(stderr, "[-] Connection failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
#else
        perror("[-] Connection failed");
        close(sockfd);
#endif
        return 1;
    }

    log_message(LOG_INFO, "[ok] Connected to server");

    char buffer[MAX_COMMAND_LENGTH];
    int my_id = -1;

    // Handshake: wait for ID_ASSIGN
    while (1) {
        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) break;

        buffer[received] = '\0';
        ParsedCommand cmd;
        if (parse_command(buffer, &cmd) == 0) {
            if (strcmp(cmd.status, "READY") == 0 && strcmp(cmd.message, "ID_ASSIGN") == 0) {
                my_id = cmd.dest_id;
                log_message(LOG_INFO, "Assigned client ID: %d", my_id);
                break;
            }
        }
    }

    if (my_id < 0) {
        log_message(LOG_ERROR, "Failed to receive ID assignment.");
        return 1;
    }

    init_chat_buffers();  // Initialize chunk reassembly buffers

    // Launch listener thread
    thread_t listener_thread;
    create_thread(&listener_thread, client_listener, &sockfd);
    detach_thread(listener_thread);

    // Main loop: user input
    while (client_running) {
        int target_id = -1;
        char message[MAX_MESSAGE_LENGTH];

        printf("\n[SEND] Enter target client ID: ");
        fflush(stdout);
        if (scanf("%d", &target_id) != 1) break;
        getchar(); // consume newline

        const char* channel = "chat";
        if (cfg.port == cfg.port_file) channel = "file";
        else if (cfg.port == cfg.port_game) channel = "game";

        if (strcmp(channel, "chat") == 0) {
            printf("[CHAT] Enter message: ");
            fflush(stdout);
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = '\0';

            if (strlen(message) == 0) continue;
            send_chat(sockfd, my_id, target_id, message);

        } else if (strcmp(channel, "file") == 0) {
            printf("[FILE] Enter filename to send (from assets/to_send/): ");
            fflush(stdout);
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = '\0';

            if (strlen(message) == 0) continue;

            build_frame("file", my_id, target_id, message, "REQUEST", buffer);
            send(sockfd, buffer, strlen(buffer), 0);
            log_message(LOG_INFO, "File request sent to client %d for '%s'", target_id, message);

        } else if (strcmp(channel, "game") == 0) {
            printf("[GAME] Game feature not yet supported.\n");
            log_message(LOG_WARN, "Game feature is stubbed.");
        }
    }

    client_running = 0;

#ifdef _WIN32
    closesocket(sockfd);
    WSACleanup();
#else
    close(sockfd);
#endif

    return 0;
}

/**
 * @brief Main function wrapper.
 */
int main(int argc, char** argv) {
    return run_client(argc, argv);
}
