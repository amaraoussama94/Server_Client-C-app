/**
 * @file parser.c
 * @brief Implements command parsing and validation logic.
 *       Validates CRC integrity before accepting commands.
 *      Format: <CRC>|<OPTION>|<PAYLOAD>|EOC
 *      Logs parsing errors and CRC mismatches.
 *     Future: Extend validation for channels, IDs, message formats.
 *   @date 2025-09-14
 *  @author Oussama Amara
 * @version 0.7
 */
#include "protocol.h"
#include "crc.h"
#include "logger.h"

int parse_command(const char* input, ParsedCommand* cmd) {
    if (decode_frame(input, cmd) != 0) {
        log_message(LOG_WARN, "Frame decoding failed or EOM missing.");
        return -1;
    }

    if (!validate_crc(cmd->crc, cmd->message)) {
        log_message(LOG_WARN, "CRC mismatch: received=%s", cmd->crc);
        return -1;
    }

    // Future: validate channel, message format, ID ranges, etc.
    log_message(LOG_DEBUG, "Command validated: channel=%s src=%d dest=%d msg=%s",
                cmd->channel, cmd->src_id, cmd->dest_id, cmd->message);
    return 0;
}

void free_parsed_command(ParsedCommand* cmd) {
    (void)cmd;
}
