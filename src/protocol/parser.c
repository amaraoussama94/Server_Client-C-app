/**
 * @file parser.c
 * @brief Parses raw input into structured command format.
 */

#include "protocol.h"
#include "logger.h"
#include <string.h>

int parse_command(const char* input, ParsedCommand* cmd) {
    if (!input || !cmd) return -1;

    char temp[MAX_COMMAND_LENGTH];
    strncpy(temp, input, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char* token = strtok(temp, " ");
    if (!token) return -1;

    strncpy(cmd->command, token, sizeof(cmd->command) - 1);
    cmd->command[sizeof(cmd->command) - 1] = '\0';

    int i = 0;
    while ((token = strtok(NULL, " ")) && i < MAX_COMMAND_ARGS) {
        strncpy(cmd->args[i], token, sizeof(cmd->args[i]) - 1);
        cmd->args[i][sizeof(cmd->args[i]) - 1] = '\0';
        i++;
    }

    cmd->arg_count = i;
    log_message(LOG_DEBUG, "Parsed command: %s", cmd->command);
    return 0;
}

void free_parsed_command(ParsedCommand* cmd) {
    // No dynamic memory used
    (void)cmd;
}
