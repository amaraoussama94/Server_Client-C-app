/**
 * @file chat.c
 * @brief Implements server-side chat message handling.
 *      Sends chat messages to the intended recipient.
 * @date 2025-09-14
 * @author Oussama Amara
 * @version 1.0
 */

#include "chat.h"
#include "logger.h"

void send_chat(int connfd, const char* message) {
    send(connfd, message, strlen(message), 0);
    log_message(LOG_INFO, "Chat message sent.");
}
