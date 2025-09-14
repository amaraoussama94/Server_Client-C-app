/**
 * @file dispatcher.c
 * @brief Routes parsed commands to appropriate handlers based on channel and status.
 *        Supports chat, file, game, and system logic with delivery confirmation.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-09-14
 */

#include "dispatcher.h"
#include "protocol.h"
#include "client_registry.h"
#include "logger.h"
#include <string.h>
#include <unistd.h>

void dispatch_command(const ParsedCommand* cmd) {
    if (!cmd) return;
    // Update last activity timestamp
    update_activity(cmd->src_id);
    // Handle system commands like delivery confirmation
    if (strcmp(cmd->channel, "system") == 0) {
        // Acknowledgment handling
        if (strcmp(cmd->status, "ACK") == 0) {
            int sender_fd = get_socket_by_id(cmd->dest_id);
            //if sender_fd is valid, send confirmation back
            if (sender_fd >= 0) {
                char ack_msg[MAX_COMMAND_LENGTH];
                build_frame("system", 0, cmd->dest_id, "DELIVERY_CONFIRMED", "ACK", ack_msg);
                send(sender_fd, ack_msg, strlen(ack_msg), 0);
            }
        }
        return;
    }

    int receiver_fd = get_socket_by_id(cmd->dest_id);
    if (receiver_fd < 0) {
        log_message(LOG_WARN, "Receiver %d not found.", cmd->dest_id);
        return;
    }

    if (strcmp(cmd->channel, "chat") == 0) {
        char forward[MAX_COMMAND_LENGTH];
        build_frame("chat", cmd->src_id, cmd->dest_id, cmd->message, "READY", forward);
        send(receiver_fd, forward, strlen(forward), 0);
    } else if (strcmp(cmd->channel, "file") == 0) {
        char wait_msg[MAX_COMMAND_LENGTH];
        build_frame("file", 0, cmd->dest_id, "Prepare for file", "WAIT", wait_msg);
        send(receiver_fd, wait_msg, strlen(wait_msg), 0);

        sleep(1); // Simulate transfer delay

        char ready_msg[MAX_COMMAND_LENGTH];
        build_frame("file", cmd->src_id, cmd->dest_id, cmd->message, "READY", ready_msg);
        send(receiver_fd, ready_msg, strlen(ready_msg), 0);

        char done_msg[MAX_COMMAND_LENGTH];
        build_frame("file", 0, cmd->dest_id, "Transfer complete", "DONE", done_msg);
        send(receiver_fd, done_msg, strlen(done_msg), 0);
    } else if (strcmp(cmd->channel, "game") == 0) {
        char ack_msg[MAX_COMMAND_LENGTH];
        build_frame("game", 0, cmd->src_id, "Game logic not implemented", "ACK", ack_msg);
        send(get_socket_by_id(cmd->src_id), ack_msg, strlen(ack_msg), 0);
    } else {
        log_message(LOG_WARN, "Unknown channel: %s", cmd->channel);
    }
}
