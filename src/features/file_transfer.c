/**
 * @file file_transfer.c
 * @brief Implements server-side file transfer logic.
 */

#include "file_transfer.h"
#include "logger.h"

void send_file_to_client(int* connfd, const char* filename, struct sockaddr_in cli) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        log_message(LOG_ERROR, "File not found: %s", filename);
        return;
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(*connfd, buffer, bytes, 0);
    }

    fclose(fp);
    log_message(LOG_INFO, "File sent: %s", filename);
}
