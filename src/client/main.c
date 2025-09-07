/**
 * @file client.c
 * @brief Implements the main client logic: config loading, connection setup, and feature dispatch.
 *        Connects to a server using TCP, then sends chat and file requests based on configuration.
 *        Uses cross-platform socket APIs and structured logging for traceability.
 *        Formats messages using CRC|OPTION|PAYLOAD|EOC protocol.
 * @author Oussama Amara
 * @version 0.7
 * @date 2025-09-07
 */

#include "client.h"
#include "config.h"
#include "logger.h"
#include "chat.h"
#include "file_client.h"
#include "protocol.h"
#include "crc.h"

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

/**
 * @brief Formats a message using CRC|OPTION|PAYLOAD|EOC and sends it over the socket.
 * @param sockfd Socket descriptor.
 * @param option Feature type: "msg", "file", "game".
 * @param payload Message or file path.
 */
void send_formatted(int sockfd, const char* option, const char* payload) {
    char crc[8];
    generate_crc(payload, crc);

    char message[1024];
    snprintf(message, sizeof(message), "%s|%s|%s|EOC", crc, option, payload);

    send(sockfd, message, strlen(message), 0);
    log_message(LOG_INFO, "Sent formatted message: %s", message);
}

/**
 * @brief Runs the client application.
 *        Loads configuration, connects to server, and dispatches enabled features.
 * @param argc Argument count.
 * @param argv Argument vector. Expects argv[1] to be path to config file.
 * @return 0 on success, non-zero on failure.
 */
int run_client(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <config_path>\n", argv[0]);
        return 1;
    }

    log_message(LOG_INFO, "Attempting to load config from: %s", argv[1]);

    Config cfg;
    if (load_config(argv[1], &cfg) != 0) {
        fprintf(stderr, "[-] Failed to load client config.\n");
        return 1;
    }

    set_log_level(LOG_INFO);
    log_message(LOG_INFO, "Connecting to server at %s:%d", cfg.host, cfg.port);

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

    /**
     * @brief Dispatch enabled features based on config.
     */
    if (cfg.enable_chat) {
        send_formatted(sockfd, "msg", "Hello from client!");
    }

    if (cfg.enable_file) {
        send_formatted(sockfd, "file", "example.txt");
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
 * @brief Entry point for the client application.
 *        Delegates to run_client().
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code from run_client().
 */
int main(int argc, char** argv) {
    return run_client(argc, argv);
}
