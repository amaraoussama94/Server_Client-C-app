/**
 * @file file_transfer.c
 * @brief Implements server-side file transfer logic.
 */

#include "file_transfer.h"
#include "logger.h"

void send_file_to_client(int* connfd, const char* filename, struct sockaddr_in cli) {
    if (!connfd || !filename) return;

    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        log_message(LOG_ERROR, "File not found: %s", filename);
        return;
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (send(*connfd, buffer, bytes, 0) < 0) {
            log_message(LOG_ERROR, "Failed to send file chunk.");
            break;
        }
    }

    fclose(fp);
    log_message(LOG_INFO, "File sent: %s", filename);
}
