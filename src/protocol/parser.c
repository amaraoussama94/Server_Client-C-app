/**
 * @file parser.c
 * @brief Parses raw input into structured command format.
 */

#include "protocol.h"
#include "logger.h"
#include <string.h>
#include <stdlib.h>

ParsedCommand* parse_command(const char* input) {
    ParsedCommand* cmd = malloc(sizeof(ParsedCommand));
    if (!cmd) return NULL;

    char* token = strtok((char*)input, " ");
    cmd->command = strdup(token);

    int i = 0;
    while ((token = strtok(NULL, " ")) && i < MAX_ARGS) {
        cmd->args[i++] = strdup(token);
    }
    cmd->arg_count = i;

    log_message(LOG_DEBUG, "Parsed command: %s", cmd->command);
    return cmd;
}
