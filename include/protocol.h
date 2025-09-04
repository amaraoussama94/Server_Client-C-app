/**
 * @file protocol.h
 * @brief Defines the message protocol and parsing utilities for client-server communication.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_COMMAND_ARGS 10
#define MAX_COMMAND_LENGTH 1024

/**
 * @brief Represents a parsed command from the client.
 */
typedef struct {
    char command[64];
    char args[MAX_COMMAND_ARGS][256];
    int arg_count;
} ParsedCommand;

/**
 * @brief Parses a raw message into a structured command.
 * @param[in] raw_msg Raw input string from client.
 * @param[out] cmd Parsed command structure.
 * @return 0 on success, -1 on failure.
 */
int parse_command(const char* raw_msg, ParsedCommand* cmd);

#endif // PROTOCOL_H
