/**
 * @file file_transfer.h
 * @brief Unified file transfer interface for client and server.
 *        Handles sending, receiving, chunk framing, reassembly, and delivery confirmation.
 *        Used by both dispatcher and client listener threads.
 * @author Oussama Amara
 * @version 1.2
 * @date 2025-09-28
 */

#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include "protocol.h"
#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sends a file to the connected client in chunked protocol frames.
 *        Each chunk includes CRC, sequence number, and END flag.
 *        Finalizes with a DONE frame.
 * @param connfd Pointer to connected socket descriptor.
 * @param filename Name of the file to send (relative to assets/to_send/).
 * @param src_id Sender client ID.
 * @param dest_id Receiver client ID.
 */
void send_file_to_client(int* connfd, const char* filename, int src_id, int dest_id);

/**
 * @brief Handles incoming file transfer notification (INCOMING).
 *        Sends a READY frame to the server to initiate chunk delivery.
 * @param cmd Parsed INCOMING command.
 * @param sockfd Connected socket descriptor.
 */
void handle_file_incoming(const ParsedCommand* cmd, int sockfd);

/**
 * @brief Buffers incoming file chunks and reassembles the file.
 *        Saves the file to assets/received/ and sends ACK or ERR to sender.
 * @param cmd Parsed CHUNK command.
 * @param sockfd Connected socket descriptor.
 */
void handle_file_chunk(const ParsedCommand* cmd, int sockfd);

#ifdef __cplusplus
}
#endif

#endif // FILE_TRANSFER_H
