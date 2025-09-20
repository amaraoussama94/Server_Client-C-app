/**
 * @file protocol.h
 * @brief Defines protocol framing and command structure for feature-aware communication.
 *       Supports chat, file transfer, game moves, and system commands with CRC validation.
 *      Uses custom protocol format: <CRC>|<CHANNEL>|<SRC_ID>|<DEST_ID>|<MESSAGE>|<STATUS>
 *      Example: 1A2B3C4D|chat|1|2|Hello there!|READY
 *      Frame status can be WAIT, READY, DONE, ACK, ERR, etc.
 *     Ensures message integrity and proper routing between clients and server.
 * @date 2025-09-14
 * @author Oussama Amara
 * @version 1.0
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#define MAX_COMMAND_LENGTH 1024
#define MAX_MESSAGE_LENGTH 512

typedef struct {
    char crc[9];           ///< CRC checksum for message integrity
    char channel[16];      ///< Feature type: chat, file, game, system
    int src_id;            ///< Sender ID (0 = server)
    int dest_id;           ///< Receiver ID
    char message[MAX_MESSAGE_LENGTH]; ///< Message content
    char status[16];       ///< Frame status: WAIT, READY, DONE, ACK, ERR, etc.
    int seq_num;     ///< Sequence number of chunk
    int is_final;    ///< 1 if last chunk, 0 otherwise
} ParsedCommand;

/**
 * @brief Builds a protocol frame from components.
 * @param channel Feature type: chat, file, game, system
 * @param src_id Sender ID (0 = server)
 * @param dest_id Receiver ID
 * @param message Message content
 * @param status Frame status: WAIT, READY, DONE, ACK, ERR, etc.
 * @param out_frame Output buffer for the constructed frame
 * @return void
 */
void build_frame(const char* channel, int src_id, int dest_id,
                 const char* message, const char* status, char* out_frame);

/**
 * @brief Decodes a raw frame into ParsedCommand structure.
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
