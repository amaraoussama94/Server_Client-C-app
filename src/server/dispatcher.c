/**
 * @file dispatcher.c
 * @brief Routes parsed commands to appropriate handlers based on channel and status.
 *        Supports chat, file, game, and system logic with delivery confirmation.
 *        Delegates file logic to features/file_transfer.c.
 *        Logs key events including ACK receipt, file size, and chunk count.
 * @date 2025-10-19
 * @author Oussama
 * @version 2.2
 */

#include "dispatcher.h"
#include "protocol.h"
#include "client_registry.h"
#include "logger.h"
#include "chat.h"
#include "file_transfer.h"

#include <string.h>
#include <unistd.h>

/**
 * @brief Dispatches a parsed command to its appropriate handler.
 *        Handles chat, file, game, and system channels.
 *        Logs delivery confirmation and file transfer lifecycle.
 * @param cmd Pointer to parsed command.
 */
void dispatch_command(const ParsedCommand* cmd) {
    if (!cmd) return;
    log_message(LOG_DEBUG, "Dispatching → channel=%s src=%d dest=%d status=%s msg=%s",
                cmd->channel, cmd->src_id, cmd->dest_id, cmd->status, cmd->message);

    update_activity(cmd->src_id);

    // ─────────────────────────────────────────────
    // System-level ACK handling
    // ─────────────────────────────────────────────
    if (strcmp(cmd->channel, "system") == 0) {
        if (strcmp(cmd->status, "ACK") == 0) {
            int sender_fd = get_socket_by_id(cmd->dest_id);
            if (sender_fd >= 0) {
                char ack_msg[MAX_COMMAND_LENGTH];
                build_frame("system", 0, cmd->dest_id, "DELIVERY_CONFIRMED", "ACK", ack_msg);
                send(sender_fd, ack_msg, strlen(ack_msg), 0);
                //to do fix routing for ack file
                if (strcmp(cmd->channel, "file") == 0) {
                    log_message(LOG_INFO, "[FILE] ACK received from client %d and confirmation sent to client %d",
                                 cmd->dest_id ,cmd->src_id);   
                } else {  
                log_message(LOG_INFO, "[SYSTEM] ACK received from client %d and confirmation sent to client %d",
                            cmd->src_id, cmd->dest_id);
                }
            }
        }
        return;
    }

    // ─────────────────────────────────────────────
    // Chat message routing
    // ─────────────────────────────────────────────
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

            log_message(LOG_INFO, "[CHAT] Forwarding from %d to %d: %s", cmd->src_id, cmd->dest_id, full_msg);
            int sent = send(dest_fd, forward, strlen(forward), 0);
            if (sent <= 0) {
                log_message(LOG_ERROR, "Failed to send to client %d", cmd->dest_id);
            }
        }
        return;
    }

    // ─────────────────────────────────────────────
    // File transfer routing
    // ─────────────────────────────────────────────
    if (strcmp(cmd->channel, "file") == 0) {
        int dest_fd = get_socket_by_id(cmd->dest_id);

        if (strcmp(cmd->status, "REQUEST") == 0) {
            if (dest_fd <= 0) {
                log_message(LOG_ERROR, "[FILE] Target client %d not available", cmd->dest_id);
                return;
            }

            char notify[MAX_COMMAND_LENGTH];
            build_frame("file", 0, cmd->dest_id, cmd->message, "INCOMING", notify);
            send(dest_fd, notify, strlen(notify), 0);
            log_message(LOG_INFO, "[FILE] Notified client %d of incoming file '%s' from client %d",
                        cmd->dest_id, cmd->message, cmd->src_id);
        }

        else if (strcmp(cmd->status, "READY") == 0) {
            int receiver_fd = get_socket_by_id(cmd->src_id);  // src_id is the one who sent READY
            if (receiver_fd <= 0) {
                log_message(LOG_ERROR, "[FILE] Destination client %d not available for delivery", cmd->src_id);
                return;
            }

            log_message(LOG_INFO, "[FILE] Client %d is ready to receive '%s' from client %d",
                        cmd->src_id, cmd->message, cmd->dest_id);
            send_file_to_client(&receiver_fd, cmd->message, cmd->dest_id, cmd->src_id);
        }

        else if (strcmp(cmd->status, "ACK") == 0) {
            log_message(LOG_INFO, "[FILE] Received ACK for '%s' from client %d", cmd->message, cmd->src_id);
        }

        return;
    }

    // ─────────────────────────────────────────────
    // Game logic stub
    // ─────────────────────────────────────────────
    if (strcmp(cmd->channel, "game") == 0) {
        log_message(LOG_WARN, "[GAME] Game feature not yet implemented.");
        return;
    }
}
