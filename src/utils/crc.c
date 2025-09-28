/**
 * @file crc.c
 * @brief Implements CRC generation and validation for protocol integrity.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-08-15
 */

#include "crc.h"
#include <stdio.h>
#include <string.h>

void generate_crc(const char* input, char* out_crc) {
    unsigned char crc = 0;
    for (size_t i = 0; i < strlen(input); ++i) {
        crc ^= input[i];
    }
    snprintf(out_crc, 8, "%02X", crc);
}

int validate_crc(const char* received_crc, const char* payload) {
    char expected[8];
    generate_crc(payload, expected);
    return strcmp(received_crc, expected) == 0;
}
