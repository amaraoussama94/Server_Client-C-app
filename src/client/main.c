/**
 * @file client.c
 * @brief Implements the client logic: config loading, connection setup, and message dispatch.
 *        Connects to a server using TCP and sends a message based on the configured port.
 *        Uses CRC|OPTION|PAYLOAD|EOC protocol format.
 * @author Oussama Amara
 * @version 0.9
 * @date 2025-09-07
 */

#include "client.h"
#include "config.h"
#include "logger.h"
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

void send_formatted(int sockfd, const char* option, const char* payload) {
    char crc[8];
    generate_crc(payload, crc);

    char message[1024];
    snprintf(message, sizeof(message), "%s|%s|%s|EOC", crc, option, payload);

    send(sockfd, message, strlen(message), 0);
    log_message(LOG_INFO, "Sent formatted message: %s", message);
}

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
    log_message(LOG_INFO, "Connecting to %s:%d", cfg.host, cfg.port);

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

    // Dispatch based on port
    if (cfg.port == cfg.port_chat) {
        send_formatted(sockfd, "msg", "Hello from client!");
    } else if (cfg.port == cfg.port_file) {
        send_formatted(sockfd, "file", "example.txt");
    } else if (cfg.port == cfg.port_game) {
        send_formatted(sockfd, "game", "start");
    } else {
        log_message(LOG_WARN, "Unknown feature port: %d", cfg.port);
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
