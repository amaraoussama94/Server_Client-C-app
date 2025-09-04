/**
 * @file chat.c
 * @brief Implements server-side chat message handling.
 */

#include "chat.h"
#include "logger.h"

void send_chat(int connfd, const char* message) {
    send(connfd, message, strlen(message), 0);
    log_message(LOG_INFO, "Chat message sent.");
}
