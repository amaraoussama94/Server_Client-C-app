/**
 * @file game.h
 * @brief Defines game-related logic and interactions for multiplayer sessions or mini-games over the network.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef GAME_H
#define GAME_H

#include "protocol.h"
/**
 * @brief Initializes game state and resources.
 */
void init_game();

/**
 * @brief Processes a game command received from the client.
 * @param[in] cmd Parsed command structure.
 */
void handle_game_command(const ParsedCommand* cmd);

#endif // GAME_H
