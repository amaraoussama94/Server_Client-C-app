/**
 * @file chat.h
 * @brief Provides utilities for handling chat messages between client and server.
 *        Supports chunked transmission, reassembly, and moderation.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-09-20
 */

#ifndef CHAT_H
#define CHAT_H

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

#include "protocol.h"

/**
 * @brief Sends a chat message to the client.
 * @param[in] connfd Socket descriptor.
 * @param[in] src_id Sender client ID.
 * @param[in] dest_id Receiver client ID.
 * @param[in] message Message to send.
  * @return void
 */
void send_chat(int connfd, int src_id, int dest_id, const char* message);


/**
 * @brief Receives a chat message from the client.
 * @param[in] connfd Socket descriptor.
 * @param[out] buffer Buffer to store received message.
 * @param[in] size Size of the buffer.
 * @return Number of bytes received, or -1 on error.
 */
int receive_chat(int connfd, char* buffer, int size);

/**
 * @brief Initializes internal buffers used for chunked message reassembly.
 */
void init_chat_buffers();

/**
 * @brief Buffers a chunked chat message for later reassembly.
 * @param[in] cmd ParsedCommand containing chunk data.
 */
void buffer_chat_chunk(const ParsedCommand* cmd);

/**
 * @brief Reassembles a full chat message from buffered chunks.
 * @param[in] src_id Sender client ID.
 * @param[in] dest_id Receiver client ID.
 * @return Pointer to assembled message, or NULL if incomplete.
 */
const char* assemble_chat_message(int src_id, int dest_id);

/**
 * @brief Checks a message for banned words.
 * @param[in] msg Message to scan.
 * @return 1 if banned word found, 0 otherwise.
 */
int moderate_chat_message(const char* msg);

#endif // CHAT_H
