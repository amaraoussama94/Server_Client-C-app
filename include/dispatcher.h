/**
 * @file dispatcher.h
 * @brief Header for dispatcher module to route commands to appropriate feature handlers.
 *        Supports modular dispatching for chat, file, and game features.
 *        Used by the server to route parsed commands based on port or protocol.
 * @author Oussama Amara
 * @version 0.6
 * @date 2025-09-07
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "protocol.h"

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

/**
 * @brief Dispatches a parsed command to the appropriate feature handler.
 * @param cmd Pointer to parsed command.
 * @return void
 */
void dispatch_command(const ParsedCommand* cmd);

/**
 * @brief Dispatches chat-related commands.
 * @param cmd Pointer to parsed command.
 * @param connfd Connection file descriptor.
 * @param cli Client address structure.
 */
void dispatch_chat_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli);

/**
 * @brief Dispatches file-related commands.
 * @param cmd Pointer to parsed command.
 * @param connfd Connection file descriptor.
 * @param cli Client address structure.
 */
void dispatch_file_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli);

/**
 * @brief Dispatches game-related commands.
 * @param cmd Pointer to parsed command.
 * @param connfd Connection file descriptor.
 * @param cli Client address structure.
 */
void dispatch_game_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli);

#endif // DISPATCHER_H
