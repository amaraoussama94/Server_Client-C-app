/**
 * @file game.c
 * @brief Implements game-related command handling.
 */

#include "game.h"
#include "logger.h"

void handle_game_command(const ParsedCommand* cmd) {
    log_message(LOG_INFO, "Game command received: %s", cmd->args[0]);
    // Placeholder for actual game logic
}
