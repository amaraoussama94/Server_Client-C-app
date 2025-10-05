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


/*
 * Constants for file transfer and client-server communication.
 *
 * MAX_CHUNKS         - Maximum number of chunks a file can be divided into.
 * MAX_CHUNK_SIZE     - Maximum size (in bytes) of each file chunk.
 * MAX_CLIENTS        - Maximum number of clients that can connect simultaneously.
 * MAX_MESSAGE_SIZE   - Maximum size (in bytes) of a message exchanged between client and server.
 */
#define MAX_CHUNKS 64
#define MAX_CHUNK_SIZE 256
#define MAX_CLIENTS 64
#define MAX_MESSAGE_SIZE 4096
/**
 * @struct FileBuffer
 * @brief Represents the state and data of a file being transferred.
 *
 * This structure is used to manage the reception of a file in chunks,
 * keeping track of which chunks have been received, the source of the file,
 * and associated metadata.
 *
 * @var FileBuffer::active
 * Indicates whether the file buffer is currently in use (1 for active, 0 for inactive).
 *
 * @var FileBuffer::src_id
 * Identifier for the source (e.g., client or server) sending the file.
 *
 * @var FileBuffer::filename
 * Name of the file being transferred (null-terminated string, max 255 characters).
 *
 * @var FileBuffer::chunks
 * 2D array storing the received file chunks. Each chunk can hold up to MAX_CHUNK_SIZE bytes plus a null terminator.
 *
 * @var FileBuffer::received
 * Array indicating which chunks have been received (1 for received, 0 for not received).
 *
 * @var FileBuffer::final_seq
 * Sequence number of the last chunk in the file transfer.
 *
 * @var FileBuffer::last_received
 * Timestamp of the last received chunk (time_t).
 */
typedef struct {
    int active;
    int src_id;
    char filename[256];
    char chunks[MAX_CHUNKS][MAX_CHUNK_SIZE + 1];
    int received[MAX_CHUNKS];
    int final_seq;
    time_t last_received;
} FileBuffer;

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
