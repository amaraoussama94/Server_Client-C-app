/**
 * @file game.c
 * @brief Implements game-related command handling.
 */

#include "game.h"
#include "logger.h"

void handle_game_command(const ParsedCommand* cmd) {
    if (cmd->arg_count < 1) {
        log_message(LOG_WARN, "Game command missing arguments.");
        return;
    }

    log_message(LOG_INFO, "Game command received: %s", cmd->args[0]);
    // Future: handle game logic
}

void init_game() {
    log_message(LOG_INFO, "Game system initialized.");
}
