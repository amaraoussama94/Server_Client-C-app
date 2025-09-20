/**
 * @file client.c
 * @brief Implements client logic: config loading, connection setup, ID assignment, message dispatch, and ACK handling.
 *        Uses custom protocol format: <CRC>|<CHANNEL>|<SRC_ID>|<DEST_ID>|<MESSAGE>|<STATUS>
 *        Supports chat, file, and game features based on port configuration.
 *        Chat messages are chunked and moderated via chat.c.
 * @author Oussama Amara
 * @version 1.1
 * @date 2025-09-20
 */

#include "client.h"
#include "config.h"
#include "logger.h"
#include "protocol.h"
#include "chat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    // Handshake: wait for ID_ASSIGN, LIST, and START frames
    while (1) {
        log_message(LOG_INFO, "Waiting for server response...");
        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) {
            log_message(LOG_ERROR, "No ID received from server.");
            break;
        }
        buffer[received] = '\0';
        log_message(LOG_INFO, "Received raw frame: %s", buffer);

        ParsedCommand cmd;
        if (parse_command(buffer, &cmd) == 0) {
            if (strcmp(cmd.status, "READY") == 0 && strcmp(cmd.message, "ID_ASSIGN") == 0) {
                my_id = cmd.dest_id;
                log_message(LOG_INFO, "Assigned client ID: %d", my_id);
            } else if (strcmp(cmd.status, "LIST") == 0) {
                log_message(LOG_INFO, "Active client: %s", cmd.message);
            } else if (strcmp(cmd.status, "START") == 0) {
                log_message(LOG_INFO, "Handshake complete. Ready to communicate.");
                break;
            }
        }
    }

    if (my_id < 0) {
        log_message(LOG_ERROR, "Failed to receive ID assignment.");
        return 1;
    }

    // Determine communication direction
    char mode[16];
    printf("Do you want to send or receive first? (send/receive): ");
    fgets(mode, sizeof(mode), stdin);
    mode[strcspn(mode, "\n")] = '\0';

    // Prompt for target ID and message if sending
    int target_id = -1;
    char message[MAX_MESSAGE_LENGTH];

    const char* channel = "chat";
    if (cfg.port == cfg.port_file) channel = "file";
    else if (cfg.port == cfg.port_game) channel = "game";

    if (strcmp(channel, "chat") == 0) {
        if (strcmp(mode, "send") == 0) {
            printf("Enter target client ID: ");
            scanf("%d", &target_id);
            getchar(); // Consume newline

            printf("Enter message: ");
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = '\0';

            send_chat(sockfd, message);
        }

        // Always attempt to receive after send or if mode is receive
        char response[MAX_MESSAGE_LENGTH];
        int received = receive_chat(sockfd, response, sizeof(response));
        if (received == -1) {
            log_message(LOG_ERROR, "Connection lost during chat receive.");
        } else if (received == -2) {
            log_message(LOG_WARN, "Message blocked due to inappropriate content.");
        } else {
            log_message(LOG_INFO, "Received chat response: %s", response);
            printf("[RECEIVED] %s\n", response);
        }
    } else {
        // Fallback for file/game channels
        printf("Enter target client ID: ");
        scanf("%d", &target_id);
        getchar(); // Consume newline

        printf("Enter message or file path: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        build_frame(channel, my_id, target_id, message, "SEND", buffer);
        log_message(LOG_INFO, "Sending frame → channel=%s to client %d: %s",
                    channel, target_id, buffer);
        send(sockfd, buffer, strlen(buffer), 0);

        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            ParsedCommand cmd;
            if (parse_command(buffer, &cmd) == 0) {
                log_message(LOG_INFO, "Parsed frame → channel=%s src=%d dest=%d status=%s msg=%s",
                            cmd.channel, cmd.src_id, cmd.dest_id, cmd.status, cmd.message);
                if (strcmp(cmd.status, "ACK") == 0 || strcmp(cmd.status, "DELIVERY_CONFIRMED") == 0) {
                    log_message(LOG_INFO, "Server confirmed delivery: %s", cmd.message);
                } else {
                    log_message(LOG_INFO, "Server response: %s [%s]", cmd.message, cmd.status);
                }
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

/**
 * @brief Main function wrapper.
 */
int main(int argc, char** argv) {
    return run_client(argc, argv);
}
