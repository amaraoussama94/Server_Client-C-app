/**
 * @file parser.c
 * @brief Parses raw input into structured command format.
 *        Supports CRC-tagged, feature-specific protocol: <CRC>|<OPTION>|<PAYLOAD>|EOC
 */
/**
 * // TODO: Add CRC validation if needed
 */
#include "protocol.h"
#include "logger.h"
#include <string.h>

/**
 * @brief Parses a structured message into ParsedCommand.
 *        Format: <CRC>|<OPTION>|<PAYLOAD>|EOC
 * @param input Raw input string.
 * @param cmd Output command structure.
 * @return 0 on success, -1 on failure.
 */
int parse_command(const char* input, ParsedCommand* cmd) {
    if (!input || !cmd) return -1;

    char temp[MAX_COMMAND_LENGTH];
    strncpy(temp, input, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char* tokens[4];
    int count = 0;

    char* token = strtok(temp, "|");
    while (token && count < 4) {
        tokens[count++] = token;
        token = strtok(NULL, "|");
    }

    if (count != 4) {
        log_message(LOG_WARN, "Malformed command: expected 4 tokens, got %d", count);
        return -1;
    }

    const char* crc = tokens[0];
    const char* option = tokens[1];
    const char* payload = tokens[2];
    const char* end = tokens[3];

    if (strcmp(end, "EOC") != 0) {
        log_message(LOG_WARN, "Invalid end-of-command marker: %s", end);
        return -1;
    }

    

    strncpy(cmd->command, option, sizeof(cmd->command) - 1);
    cmd->command[sizeof(cmd->command) - 1] = '\0';

    strncpy(cmd->args[0], payload, sizeof(cmd->args[0]) - 1);
    cmd->args[0][sizeof(cmd->args[0]) - 1] = '\0';

    cmd->arg_count = 1;

    log_message(LOG_DEBUG, "Parsed command: %s | Payload: %s", cmd->command, cmd->args[0]);
    return 0;
}

/**
 * @brief Frees ParsedCommand resources.
 *        Currently a placeholder for future dynamic fields.
 * @param cmd Pointer to ParsedCommand.
 */
void free_parsed_command(ParsedCommand* cmd) {
    (void)cmd;
}
