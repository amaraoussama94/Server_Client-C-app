
/**
 * @file client.c
 * @brief Implements the main client logic: config loading, connection setup, and feature dispatch.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */
/**
 * @file main.c
 * @brief Entry point for the client application.
 */

#include "client.h"


#include "client.h"
#include "config.h"
#include "logger.h"
#include "chat.h"
#include "file_client.h"
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_client(int argc, char** argv) {
    Config cfg;
    if (!load_config(argv[1], &cfg)) {
        fprintf(stderr, "[-] Failed to load client config.\n");
        return 1;
    }

    set_log_level(LOG_INFO);
    log_message(LOG_INFO, "Connecting to server at %s:%d", cfg.host, cfg.port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-] Socket creation failed");
        return 1;
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(cfg.port);
    servaddr.sin_addr.s_addr = inet_addr(cfg.host);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("[-] Connection failed");
        return 1;
    }

    log_message(LOG_INFO, "[✓] Connected to server");

    // Example: send a chat message
    send_chat( sockfd,"Hello from client!");

    // Example: request a file
    request_file("example.txt", sockfd);

    #ifdef _WIN32
        closesocket(sockfd);
    #else
        close(sockfd);
    #endif

    return 0;
}

int main(int argc, char** argv) {
    return run_client(argc, argv);
}
