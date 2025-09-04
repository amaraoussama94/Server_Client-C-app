/**
 * @file file_client.c
 * @brief Handles file transfer requests from the client side.
 */

#include "file_client.h"
#include "protocol.h"
#include "logger.h"

void request_file(const char* filename, int sockfd) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "file %s", filename);
    send(sockfd, buffer, strlen(buffer), 0);
    log_message(LOG_INFO, "Requested file: %s", filename);
}
