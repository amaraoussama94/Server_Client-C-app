/**
 * @file dispatcher.c
 * @brief Implements dispatching logic for parsed commands.
 *        Routes commands to chat, file, or game handlers based on context or port.
 *        Supports both unified and feature-specific dispatching.
 * @author Oussama Amara
 * @version 0.6
 * @date 2025-09-07
 */

#include "dispatcher.h"
#include "protocol.h"
#include "chat.h"
#include "game.h"
#include "file_transfer.h"
#include "logger.h"

/**
 * @brief Legacy dispatcher for unified command routing.
 *        Matches command name and routes to appropriate handler.
 * @param cmd Parsed command structure.
 * @param connfd Connection file descriptor.
 * @param cli Client address structure.
 */
void dispatch_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli) {
    if (!cmd) return;

    if (cmd->arg_count < 1) {
        log_message(LOG_WARN, "Command '%s' missing arguments.", cmd->command);
        return;
    }

    if (strcmp(cmd->command, "chat") == 0) {
        send_chat(connfd, cmd->args[0]);
    } else if (strcmp(cmd->command, "file") == 0) {
        send_file_to_client(&connfd, cmd->args[0], cli);
    } else if (strcmp(cmd->command, "game") == 0) {
        handle_game_command(cmd);
    } else {
        log_message(LOG_WARN, "Unknown command: %s", cmd->command);
    }
}

/**
 * @brief Dispatches chat commands received on chat port.
 * @param cmd Parsed command structure.
 * @param connfd Connection file descriptor.
 * @param cli Client address structure.
 */
void dispatch_chat_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli) {
    if (!cmd || cmd->arg_count < 1) {
        log_message(LOG_WARN, "[chat] Missing arguments.");
        return;
    }
    send_chat(connfd, cmd->args[0]);
}

/**
 * @brief Dispatches file commands received on file port.
 * @param cmd Parsed command structure.
 * @param connfd Connection file descriptor.
 * @param cli Client address structure.
 */
void dispatch_file_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli) {
    if (!cmd || cmd->arg_count < 1) {
        log_message(LOG_WARN, "[file] Missing filename argument.");
        return;
    }
    send_file_to_client(&connfd, cmd->args[0], cli);
}

/**
 * @brief Dispatches game commands received on game port.
 * @param cmd Parsed command structure.
 * @param connfd Connection file descriptor.
 * @param cli Client address structure.
 */
void dispatch_game_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli) {
    if (!cmd || cmd->arg_count < 1) {
        log_message(LOG_WARN, "[game] Missing game command argument.");
        return;
    }
    handle_game_command(cmd);
}
