/**
 * @file crc.h
 * @brief Provides CRC generation and validation utilities for protocol integrity.
 *        Used to generate and verify checksums for client-server messages.
 * @author Oussama Amara
 * @version 0.1
 * @date 2025-09-07
 */

#ifndef CRC_H
#define CRC_H

/**
 * @brief Computes a simple CRC checksum from input string.
 *        Uses XOR-based checksum for demonstration.
 * @param input Input string to hash.
 * @return Hex string representing CRC.
 */
void generate_crc(const char* input, char* out_crc);

/**
 * @brief Validates CRC against payload.
 * @param received_crc CRC received in message.
 * @param payload Payload to verify.
 * @return 1 if valid, 0 if mismatch.
 */
int validate_crc(const char* received_crc, const char* payload);

#endif // CRC_H
