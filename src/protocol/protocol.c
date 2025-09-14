/**
 * @file protocol.c
 * @brief Implements protocol frame construction and parsing.
 *        Supports structured parsing of CRC-tagged, feature-specific commands.
 *       Format: <CRC>|<OPTION>|<PAYLOAD>|EOC
 *       Validates CRC integrity before dispatching.
 * @date 2025-09-14
 * @author Oussama Amara
 * @version 0.7
 */

 #include "protocol.h"
#include "crc.h"
#include <stdio.h>
#include <string.h>

void build_frame(const char* channel, int src_id, int dest_id, const char* message, char* out_frame) {
    char crc[9];
    generate_crc(message, crc);
    snprintf(out_frame, MAX_COMMAND_LENGTH, "%s|%s|%d|%d|%s|EOM", crc, channel, src_id, dest_id, message);
}

int decode_frame(const char* input, ParsedCommand* cmd) {
    if (!input || !cmd) return -1;

    char temp[MAX_COMMAND_LENGTH];
    strncpy(temp, input, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    // decompose the input into tokens
    char* tokens[6];
    int count = 0;
    char* token = strtok(temp, "|");
    while (token && count < 6) {
        tokens[count++] = token;
        token = strtok(NULL, "|");
    }

    // Expecting exactly 6 tokens: <CRC>|<OPTION>|<SRC_ID>|<DEST_ID>|<PAYLOAD>|EOM
    if (count != 6 || strcmp(tokens[5], "EOM") != 0) return -1;

    strncpy(cmd->crc, tokens[0], sizeof(cmd->crc) - 1);
    cmd->crc[sizeof(cmd->crc) - 1] = '\0';

    strncpy(cmd->channel, tokens[1], sizeof(cmd->channel) - 1);
    cmd->channel[sizeof(cmd->channel) - 1] = '\0';

    cmd->src_id = atoi(tokens[2]);
    cmd->dest_id = atoi(tokens[3]);

    strncpy(cmd->message, tokens[4], sizeof(cmd->message) - 1);
    cmd->message[sizeof(cmd->message) - 1] = '\0';

    return validate_crc(cmd->crc, cmd->message);
}
