/**
 * @file game.c
 * @brief Implements game-related command handling.
 *       Currently a placeholder for future game logic.
 * @date 2025-09-14
 * @author Oussama Amara
 * @version 1.0
 */

#include "game.h"
#include "logger.h"

void handle_game_command(const ParsedCommand* cmd) {

    log_message(LOG_INFO, "Game command received");
}

void init_game() {
    log_message(LOG_INFO, "Game system initialized.");
}
