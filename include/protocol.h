/**
 * @file protocol.h
 * @brief Defines the message protocol and parsing utilities for client-server communication.
 *        Supports structured parsing of CRC-tagged, feature-specific commands.
 *        Format: <CRC>|<OPTION>|<PAYLOAD>|EOC
 * @author Oussama Amara
 * @version 0.6
 * @date 2025-09-07
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

/**
 * @brief Maximum number of arguments in a command.
 * @brief Maximum length of a command string.
 */
#define MAX_COMMAND_ARGS 10
#define MAX_COMMAND_LENGTH 1024

/**
 * @brief Represents a parsed command from the client.
 *        Includes command type and arguments.
 */
typedef struct {
    char command[64];                     ///< Feature type (e.g., "msg", "file", "game")
    char args[MAX_COMMAND_ARGS][256];    ///< Payload arguments
    int arg_count;                       ///< Number of arguments
} ParsedCommand;

/**
 * @brief Parses a raw message into a structured command.
 *        Expected format: <CRC>|<OPTION>|<PAYLOAD>|EOC
 * @param[in] raw_msg Raw input string from client.
 * @param[out] cmd Parsed command structure.
 * @return 0 on success, -1 on failure.
 */
int parse_command(const char* raw_msg, ParsedCommand* cmd);

/**
 * @brief Frees memory allocated for a ParsedCommand.
 *        Currently a placeholder for future dynamic fields.
 * @param cmd Pointer to ParsedCommand
 */
void free_parsed_command(ParsedCommand* cmd);

#endif // PROTOCOL_H
