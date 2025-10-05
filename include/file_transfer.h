/**
 * @file file_transfer.h
 * @brief Unified file transfer interface for client and server.
 *        Supports chunked delivery, reassembly, retry logic, timeout detection,
 *        and progress tracking. Used by dispatcher and listener threads.
 * @author Oussama Amara
 * @version 1.5
 * @date 2025-10-05
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
#define MAX_CHUNK_SIZE 256
#define MAX_CHUNKS 64
#define MAX_CLIENTS 64
#define MAX_MESSAGE_SIZE 4096
#define MAX_RETRIES 5
#define RETRY_INTERVAL 3 // seconds

/**
 * @struct FileBuffer
 * @brief Represents the state and data of a file being transferred.
 *        Used for client-side reassembly and retry tracking.
 */
typedef struct {
    int active;                        ///< 1 if transfer is active
    int src_id;                        ///< Sender ID
    char filename[128];               ///< Name of file being transferred
    char chunks[MAX_CHUNKS][MAX_CHUNK_SIZE + 1]; ///< Chunk data
    int received[MAX_CHUNKS];         ///< Flags for received chunks
    int final_seq;                    ///< Final chunk sequence number
    time_t last_received;             ///< Timestamp of last received chunk
    int retry_count[MAX_CHUNKS];      ///< Retry attempts per chunk
    time_t last_retry[MAX_CHUNKS];    ///< Last retry timestamp per chunk
} FileBuffer;

extern FileBuffer buffers[MAX_CLIENTS]; ///< Global buffer array for client-side reassembly

/**
 * @brief Sends a file to a client in chunked frames.
 *        Tracks progress and sends DONE frame on completion.
 * @param connfd Pointer to socket descriptor.
 * @param filename Name of file to send (from assets/to_send/).
 * @param src_id Sender ID.
 * @param dest_id Receiver ID.
 */
void send_file_to_client(int* connfd, const char* filename, int src_id, int dest_id);

/**
 * @brief Handles INCOMING frame and prepares client buffer.
 *        Sends READY frame to sender.
 * @param cmd Parsed command containing file metadata.
 * @param sockfd Socket descriptor to respond.
 */
void handle_file_incoming(const ParsedCommand* cmd, int sockfd);

/**
 * @brief Buffers incoming file chunks, tracks progress, and reassembles when complete.
 *        Sends ACK or ERR frame based on save success.
 *        Implements retry logic for missing chunks.
 * @param cmd Parsed command containing chunk data.
 * @param sockfd Socket descriptor to respond.
 */
void handle_file_chunk(const ParsedCommand* cmd, int sockfd);

#endif // FILE_TRANSFER_H
