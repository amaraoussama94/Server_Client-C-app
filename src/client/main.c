/**
 * @file client.c
 * @brief Implements client logic: config loading, connection setup, ID assignment, message dispatch, and ACK handling.
 *        Uses custom protocol format: <CRC>|<CHANNEL>|<SRC_ID>|<DEST_ID>|<MESSAGE>|<STATUS>
 *        Supports chat, file, and game features based on port configuration.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-09-14
 */

#include "client.h"
#include "config.h"
#include "logger.h"
#include "protocol.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

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

    log_message(LOG_INFO, "[✓] Connected to server");

    char buffer[MAX_COMMAND_LENGTH];
    int my_id = -1;

    // Receive ID assignment and client list
    while (1) {
        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) break;
        buffer[received] = '\0';

        ParsedCommand cmd;
        if (parse_command(buffer, &cmd) == 0) {
            // wait for serveur  ID_ASSIGN 
            if (strcmp(cmd.status, "READY") == 0 && strcmp(cmd.message, "ID_ASSIGN") == 0) {
                my_id = cmd.dest_id;
                log_message(LOG_INFO, "Assigned client ID: %d", my_id);
                //list active clients
            } else if (strcmp(cmd.status, "LIST") == 0) {
                log_message(LOG_INFO, "Active client: %s", cmd.message);
            } else {
                break; // Exit loop when actual message exchange begins
            }
        }
    }

    if (my_id < 0) {
        log_message(LOG_ERROR, "Failed to receive ID assignment.");
        return 1;
    }

    // Prompt user for target ID and message
    int target_id;
    char message[MAX_MESSAGE_LENGTH];

    printf("Enter target client ID: ");
    scanf("%d", &target_id);
    getchar(); // Consume newline

    printf("Enter message or file path: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';

    // Determine channel based on port
    const char* channel = "chat";
    if (cfg.port == cfg.port_file) channel = "file";
    else if (cfg.port == cfg.port_game) channel = "game";

    build_frame(channel, my_id, target_id, message, "SEND", buffer);
    send(sockfd, buffer, strlen(buffer), 0);

    // Wait for ACK or delivery confirmation
    int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        ParsedCommand cmd;
        if (parse_command(buffer, &cmd) == 0) {
            if (strcmp(cmd.status, "ACK") == 0 || strcmp(cmd.status, "DELIVERY_CONFIRMED") == 0) {
                log_message(LOG_INFO, "Server confirmed delivery: %s", cmd.message);
            } else {
                log_message(LOG_INFO, "Server response: %s [%s]", cmd.message, cmd.status);
            }
        }
    }

#ifdef _WIN32
    closesocket(sockfd);
    WSACleanup();
#else
    close(sockfd);
#endif

    return 0;
}

int main(int argc, char** argv) {
    return run_client(argc, argv);
}
