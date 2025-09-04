/**
 * @file chat_client.c
 * @brief Handles chat messages from the client side.
 */

#include "chat.h"
#include "logger.h"
#include <string.h>

void send_chat_message(const char* message, int sockfd) {
    send(sockfd, message, strlen(message), 0);
    log_message(LOG_INFO, "Sent chat message: %s", message);
}
