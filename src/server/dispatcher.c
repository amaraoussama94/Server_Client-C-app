/**
 * @file dispatcher.c
 * @brief Routes parsed commands to appropriate handlers based on channel and status.
 *        Supports chat, file, game, and system logic with delivery confirmation.
 * @date 2025-09-20
 * @author Oussama
 * @version 2.0
 */

#include "dispatcher.h"
#include "protocol.h"
#include "client_registry.h"
#include "logger.h"
#include "chat.h"
#include <string.h>
#include <unistd.h>

/**
 * @brief Dispatches a parsed command to its appropriate handler.
 * @param cmd Pointer to parsed command.
 */
void dispatch_command(const ParsedCommand* cmd) {
    if (!cmd) return;
    log_message(LOG_DEBUG, "Dispatching → channel=%s src=%d dest=%d status=%s msg=%s",
            cmd->channel, cmd->src_id, cmd->dest_id, cmd->status, cmd->message);

    update_activity(cmd->src_id);
    // Handle system commands like ACKs
    if (strcmp(cmd->channel, "system") == 0) {
        if (strcmp(cmd->status, "ACK") == 0) {
            int sender_fd = get_socket_by_id(cmd->dest_id);
            if (sender_fd >= 0) {
                char ack_msg[MAX_COMMAND_LENGTH];
                build_frame("system", 0, cmd->dest_id, "DELIVERY_CONFIRMED", "ACK", ack_msg);
                send(sender_fd, ack_msg, strlen(ack_msg), 0);
            }
        }
        return;
    }

    if (strcmp(cmd->channel, "chat") == 0) {
        buffer_chat_chunk(cmd);
        if (cmd->is_final) {
            const char* full_msg = assemble_chat_message(cmd->src_id, cmd->dest_id);
            if (!full_msg) {
                log_message(LOG_WARN, "Incomplete chat message from %d", cmd->src_id);
                return;
            }

            if (moderate_chat_message(full_msg)) {
                char alert[MAX_COMMAND_LENGTH];
                build_frame("system", 0, cmd->src_id, "Inappropriate language detected", "ALERT", alert);
                send(get_socket_by_id(cmd->src_id), alert, strlen(alert), 0);
                return;
            }

            char forward[MAX_COMMAND_LENGTH];
            build_frame("chat", cmd->src_id, cmd->dest_id, full_msg, "READY", forward);
            int dest_fd = get_socket_by_id(cmd->dest_id);
            if (dest_fd <= 0) {
                log_message(LOG_ERROR, "Invalid destination ID: %d. Cannot route message.", cmd->dest_id);
                return;
            }
            log_message(LOG_INFO, "Forwarding chat from %d to %d: %s", cmd->src_id, cmd->dest_id, full_msg);  
            int sent=send(get_socket_by_id(cmd->dest_id), forward, strlen(forward), 0);
            if (sent <= 0) {
                log_message(LOG_ERROR, "Failed to send to client %d", cmd->dest_id);
            }

        }
        return;
    }

    // file and game logic unchanged...
}
