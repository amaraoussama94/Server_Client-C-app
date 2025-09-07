/**
 * @file protocol.c
 * @brief Implements protocol logic for encoding and decoding messages.
 */

 // to do build and  encode decode  protocole should be here parse should be  rleated  to check and msg
#include "protocol.h"
#include "logger.h"
#include <string.h>
#include <stdio.h>

void encode_message(const char* raw, char* encoded) {
    snprintf(encoded, 1024, "[ENCODED]%s", raw);
    log_message(LOG_DEBUG, "Encoded message.");
}

void decode_message(const char* encoded, char* raw) {
    strncpy(raw, encoded + 9, 1024); // Skip "[ENCODED]"
    log_message(LOG_DEBUG, "Decoded message.");
}
