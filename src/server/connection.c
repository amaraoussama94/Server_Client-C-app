/**
 * @file connection.c
 * @brief Manages socket creation, binding, listening, and accepting client connections.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#include "connection.h"
#include "logger.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int win_socket_init(void) {
#ifdef _WIN32
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "[-] Failed to initialize Winsock.\n");
        return 1;
    }
#endif
    return 0;
}

void win_socket_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

void create_socket(int* sockfd) {
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0) {
        log_message(LOG_ERROR, "Socket creation failed.");
        exit(1);
    }
    log_message(LOG_INFO, "Socket created.");
}

void socket_bind(struct sockaddr_in* servaddr, int* sockfd) {
    if (bind(*sockfd, (struct sockaddr*)servaddr, sizeof(*servaddr)) < 0) {
        log_message(LOG_ERROR, "Socket bind failed.");
        exit(1);
    }
    log_message(LOG_INFO, "Socket bound.");
}

void socket_listening(int* sockfd) {
    if (listen(*sockfd, 8) != 0) {
        log_message(LOG_ERROR, "Listen failed.");
        exit(1);
    }
    log_message(LOG_INFO, "Server listening.");
}

void socket_accept(int* connfd, int* sockfd, struct sockaddr_in* cli, int* len) {
    *connfd = accept(*sockfd, (struct sockaddr*)cli, len);
    if (*connfd < 0) {
        log_message(LOG_ERROR, "Accept failed.");
        exit(1);
    }
    log_message(LOG_INFO, "Client connected.");
}
