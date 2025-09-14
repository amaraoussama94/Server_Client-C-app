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
#define MAX_MESSAGE_LENGTH 512
/**
 * @brief Represents a parsed command from the client.
 *        Includes command type and arguments.
 */
typedef struct {
    char crc[9];           // CRC checksum
    char channel[16];      // Feature: chat, file, game
    int src_id;            // Sender ID (0 = server)
    int dest_id;           // Receiver ID (0 = server)
    char message[MAX_MESSAGE_LENGTH];     // Actual payload
} ParsedCommand;

/**
 * @brief Builds a protocol frame from components.
 *       Format: <CRC>|<OPTION>|<PAYLOAD>|EOC
 * @param channel Feature channel (chat, file, game)
 * @param src_id Source ID (0 = server)
 * @param dest_id Destination ID (0 = server)
 * @param message Payload message
 * @param out_frame Output buffer for the constructed frame
 * @return void
 */
void build_frame(const char* channel, int src_id, int dest_id, const char* message, char* out_frame);

/**
 * @brief Parses and validates a protocol frame.
 *       Format: <CRC>|<OPTION>|<PAYLOAD>|EOC
 *      Validates CRC before accepting the command.
 * @param input Raw input string.
 * @param cmd Output command structure.
 * @return 0 on success, -1 on failure.
 */
int decode_frame(const char* input, ParsedCommand* cmd);


/**
 * @brief Parses and validates a command frame.
 *        Validates CRC integrity before accepting.
 * @param input Raw input string.
 * @param cmd Output command structure.
 * @return 0 on success, -1 on failure.
 */
int parse_command(const char* input, ParsedCommand* cmd);

/**
 * @brief Frees memory allocated for a ParsedCommand.
 *        Currently a placeholder for future dynamic fields.
 * @param cmd Pointer to ParsedCommand
 */
void free_parsed_command(ParsedCommand* cmd);

#endif // PROTOCOL_H
