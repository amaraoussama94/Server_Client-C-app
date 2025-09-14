#include "dispatcher.h"
#include "protocol.h"
#include "client_registry.h"
#include "logger.h"

void dispatch_command(const ParsedCommand* cmd) {
    update_activity(cmd->src_id);

    // Handle special system messages
    if (strcmp(cmd->channel, "system") == 0) {
        if (strcmp(cmd->message, "RECEIVED_OK") == 0) {
            int sender_fd = get_socket_by_id(cmd->dest_id);
            char ack_msg[MAX_COMMAND_LENGTH];
            build_frame("system", 0, cmd->dest_id, "DELIVERY_CONFIRMED", ack_msg);
            send(sender_fd, ack_msg, strlen(ack_msg), 0);
        }
        return;
    }
    // Forward to destination client
    int receiver_fd = get_socket_by_id(cmd->dest_id);
    // If receiver not found, log and drop
    if (receiver_fd < 0) {
        log_message(LOG_WARN, "Receiver %d not found.", cmd->dest_id);
        return;
    }

    char forward[MAX_COMMAND_LENGTH];
    build_frame(cmd->channel, cmd->src_id, cmd->dest_id, cmd->message, forward);
    send(receiver_fd, forward, strlen(forward), 0);
}
