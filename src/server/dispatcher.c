/**
 * @file dispatcher.c
 * @brief Dispatches parsed commands to appropriate feature handlers.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#include "dispatcher.h"
#include "protocol.h"
#include "chat.h"
#include "game.h"
#include "file_transfer.h"
#include "logger.h"

void dispatch_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli)  {
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
