/**
 * @file protocol.c
 * @brief Implements command framing and parsing logic.
 *        Builds and decodes structured protocol frames for chat, file, and game features.
 *        Format: <CRC>|<CHANNEL>|<SRC_ID>|<DEST_ID>|<MESSAGE>|<STATUS>|SEQ|END
 *        Supports chunked delivery and integrity validation.
 * @date 2025-09-28
 * @author Oussama Amara
 * @version 0.8
 */



#include "protocol.h"
#include "crc.h"
#include <string.h>
#include <stdio.h>

void build_frame(const char* channel, int src_id, int dest_id,
                 const char* message, const char* status, char* out_frame) {
    char crc[9];
    generate_crc(message, crc);
    snprintf(out_frame, MAX_COMMAND_LENGTH, "%s|%s|%d|%d|%s|%s",
             crc, channel, src_id, dest_id, message, status);
}

//Frame format: <CRC>|<CHANNEL>|<SRC_ID>|<DEST_ID>|<MESSAGE>|<STATUS>|SEQ=X|END=Y

int decode_frame(const char* input, ParsedCommand* cmd) {
    if (!input || !cmd) return -1;

    char temp[MAX_COMMAND_LENGTH];
    strncpy(temp, input, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char* tokens[8];
    int count = 0;
    char* token = strtok(temp, "|");
    while (token && count < 8) {
        tokens[count++] = token;
        token = strtok(NULL, "|");
    }

    if (count < 6) return -1;

    strncpy(cmd->crc, tokens[0], sizeof(cmd->crc) - 1);
    strncpy(cmd->channel, tokens[1], sizeof(cmd->channel) - 1);
    cmd->src_id = atoi(tokens[2]);
    cmd->dest_id = atoi(tokens[3]);
    strncpy(cmd->message, tokens[4], sizeof(cmd->message) - 1);
    strncpy(cmd->status, tokens[5], sizeof(cmd->status) - 1);
    cmd->seq_num = (count >= 7) ? atoi(tokens[6]) : 0;
    cmd->is_final = (count == 8) ? atoi(tokens[7]) : 1;

    return 0;
}
